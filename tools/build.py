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


def ghidra_label(name):
    # ApplySymbols stores C names without their leading underscore.
    return name[1:] if name.startswith("_") else name


def build_target(units):
    by_dll = {}
    for dll, d, u, full in units:
        by_dll.setdefault(dll, (d, []))[1].append((u, full))
    ok = True
    for dll, (d, items) in by_dll.items():
        args = ["/exporter", "COFF relocatable object"]
        for u, full in items:
            out = obj_path("target", full)
            os.makedirs(os.path.dirname(out), exist_ok=True)
            for name in u["functions"] + u.get("data", []):
                args += ["/include", ghidra_label(name)]
            args += ["/export", out]
        log = os.path.join(ROOT, "build", "ghidra_logs", "delink_%s.log" % dll)
        os.makedirs(os.path.dirname(log), exist_ok=True)
        cmd = [os.path.join(GHIDRA, "support", "analyzeHeadless.bat"),
               os.path.join(ROOT, "re"), "CoF" + d["ghidra_folder"],
               "-process", d["ghidra_program"], "-noanalysis",
               "-scriptPath", os.path.join(ROOT, "tools", "ghidra_scripts"),
               "-postScript", "ApplySymbols.java", os.path.join(ROOT, d["symbols"]),
               "-postScript", "DelinkProgram.java"] + args + ["-log", log]
        rc = subprocess.call(cmd, cwd=ROOT, stdout=subprocess.DEVNULL,
                             stderr=subprocess.STDOUT)
        for u, full in items:
            exists = os.path.exists(obj_path("target", full))
            print("target %-30s %s" % (full, "ok" if rc == 0 and exists else
                                       "FAILED (rc %d, see %s)" % (rc, log)))
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
