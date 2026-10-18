"""Build and diff driver.

  python tools/build.py base   [unit ...]   compile src/ units (cl 19.27.29112)
  python tools/build.py target [unit ...]   delink retail units through Ghidra
  python tools/build.py objdiff             write objdiff.json
  python tools/build.py report              objdiff report, per unit and function
  python tools/build.py all    [unit ...]   base + target + objdiff + report
  python tools/build.py check  <unit ...>   compile and report only these units
  python tools/build.py decomp <unit ...>   Ghidra pseudo C into build/decomp/ (reference)
  python tools/build.py link   <dll>        link all units of a DLL into build/bin/<dll>
                                            (only when the DLL has a "link" section and
                                            every unit compiles), then compare with retail

Units are named <dll>/<path>, for example hl/dlls/h_export. Config lives in
tools/units.json. Outputs: build/base/<dll>/<path>.obj (ours) and
build/target/<dll>/<path>.obj (retail, delinked).
"""
import csv
import json
import os
import shutil
import subprocess
import sys
import urllib.parse
import urllib.request

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import msvc  # noqa: E402
import msvc6  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GHIDRA = r"I:\Tools\ghidra_12.1.4_PUBLIC"
OBJDIFF = r"I:\Tools\bin\objdiff-cli.exe"
CONFIG = os.path.join(ROOT, "tools", "units.json")


def load_units(selected):
    cfg = json.load(open(CONFIG))
    out = []
    for dll, d in cfg["dlls"].items():
        for u in d["units"]:
            full = "%s/%s" % (dll, u["name"])
            if selected and full not in selected:
                continue
            out.append((dll, d, u, full))
    if selected and len(out) != len(selected):
        known = {o[3] for o in out}
        sys.exit("unknown unit(s): %s" % ", ".join(s for s in selected if s not in known))
    return out


def obj_path(kind, full):
    return os.path.join(ROOT, "build", kind, *full.split("/")) + ".obj"


def toolchain(d):
    """msvc1427 (default, the 1.6a game DLLs) or vc6 (engine, opengl32, GameUI)."""
    return msvc6 if d.get("toolchain") == "vc6" else msvc


def unit_functions(u):
    if "functions_file" in u:
        return [l.strip() for l in open(os.path.join(ROOT, u["functions_file"])) if l.strip()]
    return u.get("functions", [])


def build_base(units, kind="base", extra=()):
    """Compile the units into build/<kind>/. The link may add flags that
    change only how functions are packaged (link section "cflags_extra",
    objects in build/link_obj/), so the objects objdiff compares stay as
    they are."""
    ok = True
    for dll, d, u, full in units:
        tc = toolchain(d)
        tc.check()
        out = obj_path(kind, full)
        os.makedirs(os.path.dirname(out), exist_ok=True)
        # a unit may replace the dll's flags, defines or include paths
        # (third-party code built its own way, such as bzip2 in hw.dll)
        cmd = [tc.cl_path()] + u.get("cflags", d["cflags"]) + list(extra)
        cmd += ["/D" + x for x in u.get("defines", d["defines"])]
        cmd += ["/I" + os.path.join(ROOT, x) for x in u.get("includes", d["includes"])]
        cmd += ["/Fo" + out, os.path.join(ROOT, u["source"])]
        res = subprocess.run(cmd, env=tc.env(), cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        rc = res.returncode
        if rc == 0 and d.get("toolchain") == "vc6":
            # VC6 numbers anonymous namespaces at random on each compile:
            # give them the names the retail RTTI records
            import vc6_anon
            vc6_anon.fix(out)
        print("%-6s %-30s %s" % (kind[:6], full, "ok" if rc == 0 else "FAILED (%d)" % rc))
        if rc:
            # cl prints its diagnostics on stdout: show the errors
            errors = [l for l in res.stdout.decode("latin-1").splitlines() if ": error " in l or ": fatal error " in l]
            for l in errors[:40]:
                print("       " + l.replace(ROOT + os.sep, ""))
            if len(errors) > 40:
                print("       ... %d more errors" % (len(errors) - 40))
        ok &= rc == 0
    return ok


MCP_URL = "http://127.0.0.1:8089"


def mcp_up():
    try:
        with urllib.request.urlopen(MCP_URL + "/check_connection", timeout=2) as r:
            return r.status == 200
    except Exception:
        return False


def mcp_call(path, body=None, query=None, timeout=120):
    url = MCP_URL + path
    if query:
        url += "?" + urllib.parse.urlencode(query)
    data = json.dumps(body).encode() if body is not None else None
    req = urllib.request.Request(url, data=data, headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=timeout) as r:
        return json.loads(r.read().decode())


def merged_symbols(d, dll, tag=""):
    """tools/symbols/<dll>_1.6a.csv, then _decorated.csv (decorated forms of
    its undecorated C++ names), _auto.csv (harvested), then the
    curated _usermsgs.csv, _vtable.csv and _manual.csv (later files
    override earlier ones), written to build/jobs/. A tag gives the job its
    own copies, so concurrent delinks do not overwrite each other's files."""
    rows = {}
    curated = set()
    base = os.path.join(ROOT, d["symbols"])
    # base map, harvested names, then curated maps (later files win)
    for i, path in enumerate((base, base.replace("_1.6a.csv", "_decorated.csv"),
                              base.replace("_1.6a.csv", "_auto.csv"),
                              base.replace("_1.6a.csv", "_usermsgs.csv"),
                              base.replace("_1.6a.csv", "_vtable.csv"),
                              base.replace("_1.6a.csv", "_savetables.csv"),
                              base.replace("_1.6a.csv", "_bytes.csv"),
                              base.replace("_1.6a.csv", "_position.csv"),
                              base.replace("_1.6a.csv", "_manual.csv"))):
        if os.path.exists(path):
            for r in csv.DictReader(open(path)):
                va = r["va"].upper()
                rows[va] = (r["name"], r.get("kind") or "func")
                if i > 0:
                    curated.add(va)
    # a name may live at one address only (the COFF exporter refuses
    # duplicates): curated entries win, then the strongest evidence in
    # docs/progress.csv; the losers are forgotten in Ghidra
    rank = {}
    for r in csv.DictReader(open(os.path.join(ROOT, "docs", "progress.csv"))):
        if r["dll"] == dll:
            ev = r["evidence"]
            rank[r["va"].upper()] = (0 if ev.startswith("refbytes") else 1 if "exports" in ev
                                     else 2 if ev.startswith(("hl_1.55", "crt")) else 3)
    by_name = {}
    for va, (name, _kind) in rows.items():
        by_name.setdefault(name, []).append(va)
    forget = []
    for name, vas in by_name.items():
        if len(vas) > 1 and not name.startswith("$"):
            keep = [va for va in vas if va in curated]
            if len(keep) != 1:
                best = sorted(vas, key=lambda va: (rank.get(va, 9), va))
                keep = [best[0]] if rank.get(best[0], 9) < rank.get(best[1], 9) else []
            for va in vas:
                if not keep or va != keep[0]:
                    rows.pop(va, None)
                    forget.append((va, name))
    # names removed for good (tools/symbols/<dll>_forget.csv, with evidence)
    fp = base.replace("_1.6a.csv", "_forget.csv")
    if os.path.exists(fp):
        for r in csv.DictReader(open(fp)):
            va = r["va"].upper()
            if rows.get(va, ("",))[0] == r["name"]:
                rows.pop(va)
            forget.append((va, r["name"]))
    with open(os.path.join(ROOT, "build", "jobs", "forget_%s%s.csv" % (dll, tag)), "w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["va", "name"])
        w.writerows(forget)
    # sizes of named globals (tools/data_sizes.py): DelinkUnits defines
    # their extent so references into them become symbol+offset
    sizes = {}
    sp = base.replace("_1.6a.csv", "_datasizes.csv")
    if os.path.exists(sp):
        sizes = {r["name"]: r["size"] for r in csv.DictReader(open(sp))}
    out = os.path.join(ROOT, "build", "jobs", "symbols_%s%s.csv" % (dll, tag))
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, "w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["va", "name", "kind", "size"])
        for va in sorted(rows):
            name, kind = rows[va]
            w.writerow([va, name, kind, sizes.get(name, "") if kind == "data" else ""])
    if tag:
        # other tools read the shared merged map: refresh it in one step
        shared = os.path.join(ROOT, "build", "jobs", "symbols_%s.csv" % dll)
        shutil.copyfile(out, shared + tag)
        os.replace(shared + tag, shared)
    return out


def write_job(d, items, job, dll, tag=""):
    os.makedirs(os.path.dirname(job), exist_ok=True)
    with open(job, "w") as f:
        f.write("symbols %s\n" % merged_symbols(d, dll, tag))
        f.write("forget %s\n" % os.path.join(ROOT, "build", "jobs", "forget_%s%s.csv" % (dll, tag)))
        for u, full in items:
            f.write("unit %s\n" % obj_path("target", full))
            for name in unit_functions(u) + u.get("data", []):
                f.write("include %s\n" % name)


def build_target(units):
    """Delink retail units with tools/ghidra_scripts/DelinkUnits.java.

    Uses the running GhidraMCP server when it is up (it holds the project
    lock and needs GHIDRA_MCP_ALLOW_SCRIPTS=1), otherwise headless Ghidra.
    """
    by_dll = {}
    for dll, d, u, full in units:
        by_dll.setdefault(dll, (d, []))[1].append((u, full))
    script = os.path.join(ROOT, "tools", "ghidra_scripts", "DelinkUnits.java")
    use_mcp = mcp_up()
    ok = True
    for dll, (d, items) in by_dll.items():
        # one set of job files per process: agents delink concurrently
        tag = "_%d" % os.getpid()
        job = os.path.join(ROOT, "build", "jobs", "delink_%s%s.txt" % (dll, tag))
        write_job(d, items, job, dll, tag)
        for u, full in items:
            if os.path.exists(obj_path("target", full)):
                os.remove(obj_path("target", full))
        program = d["ghidra_folder"] + "/" + d["ghidra_program"]
        if use_mcp:
            res = mcp_call("/run_ghidra_script", body={
                "script_name": script, "args": job, "program": program,
                "timeout_seconds": 1800}, timeout=1900)
            mcp_call("/save_program", query={"program": program})
            rc = 0 if res.get("success") or res.get("status") == "success" else 1
            log = json.dumps(res)[:2000]
        else:
            log = os.path.join(ROOT, "build", "ghidra_logs", "delink_%s.log" % dll)
            cmd = [os.path.join(GHIDRA, "support", "analyzeHeadless.bat"),
                   os.path.join(ROOT, "re"), "CoF" + d["ghidra_folder"],
                   "-process", d["ghidra_program"], "-noanalysis",
                   "-scriptPath", os.path.dirname(script),
                   "-postScript", "DelinkUnits.java", job, "-log", log]
            rc = subprocess.call(cmd, cwd=ROOT, stdout=subprocess.DEVNULL,
                                 stderr=subprocess.STDOUT)
        for u, full in items:
            exists = os.path.exists(obj_path("target", full))
            print("target %-30s %s" % (full, "ok" if rc == 0 and exists else
                                       "FAILED (%s)" % log))
            ok &= rc == 0 and exists
        for path in (job, os.path.join(ROOT, "build", "jobs", "symbols_%s%s.csv" % (dll, tag)),
                     os.path.join(ROOT, "build", "jobs", "forget_%s%s.csv" % (dll, tag))):
            if os.path.exists(path):
                os.remove(path)
    return ok


def write_objdiff(units):
    cfg = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "build_base": False,
        "build_target": False,
        "units": [],
    }
    for dll, d, u, full in units:
        cfg["units"].append({
            "name": full,
            "target_path": os.path.relpath(obj_path("target", full), ROOT).replace("\\", "/"),
            "base_path": os.path.relpath(obj_path("base", full), ROOT).replace("\\", "/"),
            "metadata": {"source_path": u["source"]},
        })
    with open(os.path.join(ROOT, "objdiff.json"), "w") as f:
        json.dump(cfg, f, indent=2)
        f.write("\n")
    print("wrote objdiff.json (%d units)" % len(cfg["units"]))


def report():
    # objdiff refuses the whole report when one object is missing (a unit
    # that does not compile): report from a copy of the config that leaves
    # out missing objects
    cfg = json.load(open(os.path.join(ROOT, "objdiff.json")))
    proj = os.path.join(ROOT, "build", "report_project")
    os.makedirs(proj, exist_ok=True)
    for u in cfg["units"]:
        for key in ("target_path", "base_path"):
            full = os.path.join(ROOT, u[key])
            if os.path.exists(full):
                u[key] = os.path.relpath(full, proj).replace("\\", "/")
            else:
                del u[key]
    with open(os.path.join(proj, "objdiff.json"), "w") as f:
        json.dump(cfg, f, indent=2)
    out = os.path.join(ROOT, "build", "report.json")
    rc = subprocess.call([OBJDIFF, "report", "generate", "-p", proj, "-o", out,
                          "-f", "json-pretty"], cwd=ROOT)
    if rc != 0:
        return False
    rep = json.load(open(out))
    for unit in rep.get("units", []):
        m = unit.get("measures", {})
        print("%-30s code %6.2f%%  (%s/%s bytes)" % (
            unit["name"], m.get("matched_code_percent", 0.0),
            m.get("matched_code", "0"), m.get("total_code", "0")))
        for fn in unit.get("functions", []):
            print("    %7.2f%%  %s" % (fn.get("fuzzy_match_percent", 0.0), fn["name"]))
    return True


def link(dll):
    cfg = json.load(open(CONFIG))["dlls"][dll]
    if "link" not in cfg:
        sys.exit("%s has no link section in tools/units.json" % dll)
    lk = cfg["link"]
    units = load_units([])
    units = [x for x in units if x[0] == dll]
    # "order": the units in the order of the retail image (the link order)
    if "order" in lk:
        pos = {n: i for i, n in enumerate(lk["order"])}
        missing = [full for _d, _c, _u, full in units if full.split("/", 1)[1] not in pos]
        if missing:
            sys.exit("units missing from the link order: %s" % " ".join(missing))
        units.sort(key=lambda x: pos[x[3].split("/", 1)[1]])
    kind = "link_obj" if lk.get("cflags_extra") else "base"
    if not build_base(units, kind, lk.get("cflags_extra", ())):
        sys.exit("not linking: some units do not compile")
    tc = toolchain(cfg)
    tmp = os.path.join(ROOT, "build", "link", dll)
    os.makedirs(tmp, exist_ok=True)
    # "implibs": import libraries made from retail third-party DLLs. "libs" can
    # place one with "@<name>.lib" (the import table follows the library
    # order); the others go after "libs".
    made = {}
    for src, lib in lk.get("implibs", {}).items():
        path = os.path.join(tmp, lib)
        if subprocess.call([sys.executable, os.path.join(ROOT, "tools", "implib.py"),
                            os.path.join(ROOT, src), path, os.path.join(ROOT, "build", kind, dll)],
                           stdout=subprocess.DEVNULL) != 0:
            sys.exit("import library for %s failed" % src)
        made[lib] = path
    libs = []
    for lib in lk["libs"]:
        if lib.startswith("@"):
            libs.append(made.pop(lib[1:]))
        else:
            libs.append(lib)
    libs += list(made.values())
    out = os.path.join(tmp, lk["out"])
    cmd = [tc.link_path()] + lk["flags"] + ["/out:" + out, "/map:" + out + ".map"]
    cmd += [obj_path(kind, full) for _d, _c, _u, full in units] + libs
    res = subprocess.run(cmd, env=tc.env(), cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    log = res.stdout.decode("latin-1").splitlines()
    with open(out + ".log", "w") as f:
        f.write("\n".join(log) + "\n")
    msgs = [l for l in log if " error " in l or " warning " in l or "fatal error" in l]
    if res.returncode != 0:
        print("\n".join(msgs[:40]))
        sys.exit("link failed (%d), %d messages, full log build/link/%s/%s.log"
                 % (res.returncode, len(msgs), dll, lk["out"]))
    if msgs:
        print("%d linker warnings, full log build/link/%s/%s.log" % (len(msgs), dll, lk["out"]))
    bindir = os.path.join(ROOT, "build", "bin")
    os.makedirs(bindir, exist_ok=True)
    import shutil
    shutil.copyfile(out, os.path.join(bindir, lk["out"]))
    print("linked build/bin/%s" % lk["out"])
    if "retail" in lk:
        subprocess.call([sys.executable, os.path.join(ROOT, "tools", "compare_pe.py"),
                         os.path.join(bindir, lk["out"]), os.path.join(ROOT, lk["retail"])])
    return True


def check(units):
    """Compile the units and print the per-function match of only those
    units (a one-unit objdiff report): the edit-diff loop."""
    build_base(units)
    names = {full for _d, _c, _u, full in units}
    cfg = json.load(open(os.path.join(ROOT, "objdiff.json")))
    # a unit created after the last objdiff.json (tools/make_unit.py)
    if names - {u["name"] for u in cfg["units"]}:
        write_objdiff(load_units([]))
        cfg = json.load(open(os.path.join(ROOT, "objdiff.json")))
    # one project directory per unit set, so parallel checks do not collide
    tag = "_".join(sorted(n.replace("/", "_") for n in names))[:120]
    proj = os.path.join(ROOT, "build", "check_project", tag)
    os.makedirs(proj, exist_ok=True)
    keep = []
    for u in cfg["units"]:
        if u["name"] not in names:
            continue
        for key in ("target_path", "base_path"):
            full = os.path.join(ROOT, u[key])
            if os.path.exists(full):
                u[key] = os.path.relpath(full, proj).replace("\\", "/")
            else:
                del u[key]
        keep.append(u)
    cfg["units"] = keep
    with open(os.path.join(proj, "objdiff.json"), "w") as f:
        json.dump(cfg, f, indent=2)
    out = os.path.join(proj, "report.json")
    subprocess.call([OBJDIFF, "report", "generate", "-p", proj, "-o", out, "-f", "json"], cwd=ROOT,
                    stderr=subprocess.DEVNULL)
    rep = json.load(open(out))
    for unit in rep.get("units", []):
        m = unit.get("measures", {})
        print("%-30s code %6.2f%%  (%s/%s bytes)" % (unit["name"], m.get("matched_code_percent", 0.0),
                                                   m.get("matched_code", "0"), m.get("total_code", "0")))
        funcs = unit.get("functions", [])
        exact = [fn for fn in funcs if fn.get("fuzzy_match_percent", 0.0) >= 100.0]
        print("    %d of %d functions at 100%%" % (len(exact), len(funcs)))
        # only the functions below 100% (the full list costs a lot of output
        # on large units); --all prints every function
        for fn in sorted(funcs, key=lambda f: f.get("fuzzy_match_percent", 0.0)):
            pct = fn.get("fuzzy_match_percent", 0.0)
            if pct < 100.0 or SHOW_ALL:
                # floor, so 99.996 does not print as 100.00
                print("    %7.2f%%  %s" % (int(pct * 100) / 100.0, fn["name"]))


def decomp(units):
    """Ghidra pseudo C of every function of the units, into build/decomp/<unit>/
    (reference only, through the running GhidraMCP server)."""
    if not mcp_up():
        sys.exit("decomp needs the GhidraMCP server")
    script = os.path.join(ROOT, "tools", "ghidra_scripts", "DecompileList.java")
    for dll, d, u, full in units:
        job = os.path.join(ROOT, "build", "jobs", "decomp_%s.txt" % full.replace("/", "_"))
        with open(job, "w") as f:
            f.write("out %s\n" % os.path.join(ROOT, "build", "decomp", *full.split("/")))
            for name in unit_functions(u):
                f.write("func %s\n" % name)
        res = mcp_call("/run_ghidra_script", body={
            "script_name": script, "args": job,
            "program": d["ghidra_folder"] + "/" + d["ghidra_program"],
            "timeout_seconds": 1800}, timeout=1900)
        out = res.get("console_output", "")
        print("decomp %-30s %s" % (full, "ok" if "DecompileList:" in out else json.dumps(res)[:300]))


SHOW_ALL = False


def main():
    global SHOW_ALL
    if "--all" in sys.argv:
        SHOW_ALL = True
        sys.argv.remove("--all")
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    cmd, sel = sys.argv[1], sys.argv[2:]
    if cmd == "link":
        link(sel[0])
        return
    if cmd == "decomp":
        decomp(load_units(sel))
        return
    if cmd == "check":
        check(load_units(sel))
        return
    all_units = load_units([])
    units = load_units(sel) if sel else all_units
    if cmd == "base":
        ok = build_base(units)
    elif cmd == "target":
        ok = build_target(units)
    elif cmd == "objdiff":
        write_objdiff(all_units)
        ok = True
    elif cmd == "report":
        ok = report()
    elif cmd == "all":
        ok = build_base(units) and build_target(units)
        write_objdiff(all_units)
        ok = ok and report()
    else:
        sys.exit(__doc__)
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
