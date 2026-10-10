"""Build and diff driver.

  python tools/build.py base   [unit ...]   compile src/ units (cl 19.27.29112)
  python tools/build.py target [unit ...]   delink retail units through Ghidra
  python tools/build.py objdiff             write objdiff.json
  python tools/build.py report              objdiff report, per unit and function
  python tools/build.py all    [unit ...]   base + target + objdiff + report

Units are named <dll>/<path>, for example hl/dlls/h_export. Config lives in
tools/units.json. Outputs: build/base/<dll>/<path>.obj (ours) and
build/target/<dll>/<path>.obj (retail, delinked).
"""
import json
import os
import subprocess
import sys
import urllib.parse
import urllib.request

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import msvc  # noqa: E402

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


def build_base(units):
    msvc.check()
    ok = True
    for dll, d, u, full in units:
        out = obj_path("base", full)
        os.makedirs(os.path.dirname(out), exist_ok=True)
        cmd = [msvc.cl_path()] + d["cflags"]
        cmd += ["/D" + x for x in d["defines"]]
        cmd += ["/I" + os.path.join(ROOT, x) for x in d["includes"]]
        cmd += ["/Fo" + out, os.path.join(ROOT, u["source"])]
        rc = subprocess.call(cmd, env=msvc.env(), cwd=ROOT)
        print("base   %-30s %s" % (full, "ok" if rc == 0 else "FAILED (%d)" % rc))
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


def write_job(d, items, job):
    os.makedirs(os.path.dirname(job), exist_ok=True)
    with open(job, "w") as f:
        f.write("symbols %s\n" % os.path.join(ROOT, d["symbols"]))
        for u, full in items:
            f.write("unit %s\n" % obj_path("target", full))
            for name in u["functions"] + u.get("data", []):
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
        job = os.path.join(ROOT, "build", "jobs", "delink_%s.txt" % dll)
        write_job(d, items, job)
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
    out = os.path.join(ROOT, "build", "report.json")
    rc = subprocess.call([OBJDIFF, "report", "generate", "-p", ROOT, "-o", out,
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


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    cmd, sel = sys.argv[1], sys.argv[2:]
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
