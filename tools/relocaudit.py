"""Check the relocation targets of matched functions.

  python tools/relocaudit.py [<unit> ...]

The objdiff report counts a function as 100% when its instructions match,
whatever symbols the relocations point at (the report runs with relocation
names ignored). A call to the wrong function or a load of the wrong global
can hide behind such a 100%. For every function at 100% in that report,
this generates a second report with functionRelocDiffs=name_address and,
for the functions that drop below 100% there, diffs them and looks at the
rows whose relocation target names differ:

- naming: the retail side still has a Ghidra default name (FUN_, DAT_,
  s_..., switchD_, caseD_) or the two names are the same symbol spelled
  differently; the code matches, the retail map lacks a name;
- real: both sides have real names and they differ, or the same global is
  accessed at another offset (another member). Either our source uses
  something else, or the retail name is wrong (check the evidence).

Units default to every hl unit with both objects. tools/update_status.py
uses real_differences(): a function with real differences stays TODO. Prints the real
differences and a count per kind.
"""
import json
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import objdiff_tool  # noqa: E402

ROOT = objdiff_tool.ROOT
OBJDIFF = objdiff_tool.OBJDIFF
DEFAULT_NAME = re.compile(r"^(?:DAT|FUN|LAB|PTR|s|u|thunk_FUN|switchD|caseD|SUB|EXT|OFF|BYTE|WORD|DWORD)_"
                          r"|^switchD_[0-9a-fA-F]+::|^\$LN\d+$|^\.")


def same_symbol(a, b):
    """Equal names, or Ghidra's demangled spelling of a vtable
    (CFoo::vftable) against the decorated one (??_7CFoo@@6B@), or the
    same CRT function under its alias (_stricmp and __stricmp), or an
    undecorated retail name against our decorated one."""
    if a == b or a.lstrip("_") == b.lstrip("_"):
        return True
    if b.startswith("?") and b[1:].startswith(a + "@"):
        return True
    m = re.match(r"^(\w+)::vftable$", a)
    return bool(m) and b == "??_7%s@@6B@" % m.group(1)


def unit_report(unit, extra):
    cfg = json.load(open(os.path.join(ROOT, "objdiff.json")))
    proj = os.path.join(ROOT, "build", "relocaudit", unit.replace("/", "_"))
    os.makedirs(proj, exist_ok=True)
    keep = []
    for u in cfg["units"]:
        if u["name"] != unit:
            continue
        for key in ("target_path", "base_path"):
            full = os.path.join(ROOT, u[key])
            if not os.path.exists(full):
                return None
            u[key] = os.path.relpath(full, proj).replace("\\", "/")
        keep.append(u)
    cfg["units"] = keep
    with open(os.path.join(proj, "objdiff.json"), "w") as f:
        json.dump(cfg, f)
    out = os.path.join(proj, "r%s.json" % ("_names" if extra else ""))
    cmd = [OBJDIFF, "report", "generate", "-p", proj, "-o", out, "-f", "json"] + extra
    try:
        subprocess.run(cmd, cwd=ROOT, capture_output=True, timeout=300)
    except subprocess.TimeoutExpired:
        print("timeout: %s" % unit)
        return None
    rep = json.load(open(out))
    return {f["name"]: f.get("fuzzy_match_percent", 0.0) for u in rep.get("units", []) for f in u.get("functions", [])}


def real_differences(unit):
    """{function: [(retail target, our target)]} for the functions of the
    unit that are at 100% in the plain report but whose relocations point
    at other symbols (or other offsets of the same symbol); None when the
    unit has no report."""
    plain = unit_report(unit, [])
    if plain is None:
        return None
    named = unit_report(unit, ["-c", "functionRelocDiffs=name_address"])
    if named is None:
        return None
    out = {}
    for fn, pct in sorted(plain.items()):
        if pct < 100.0 or named.get(fn, 0.0) >= 100.0:
            continue
        found = differences(unit, fn)
        if found:
            out[fn] = found
    return out


def differences(unit, fn):
    d = objdiff_tool.diff_json(unit, fn)
    _l, _r, li, ri = objdiff_tool.rows(d, fn)
    found = []
    for a, b in zip(li, ri):
        na = objdiff_tool.reloc_name(d["left"], a)
        nb = objdiff_tool.reloc_name(d["right"], b)
        if not na or not nb or DEFAULT_NAME.search(na) or DEFAULT_NAME.search(nb):
            continue
        if same_symbol(na, nb):
            # the same global at another offset is another member
            xa, xb = objdiff_tool.reloc_addend(a), objdiff_tool.reloc_addend(b)
            if xa != xb and na == nb:
                found.append(("%s+0x%x" % (na, xa), "%s+0x%x" % (nb, xb)))
            continue
        found.append((na, nb))
    return sorted(set(found))


def main():
    units = sys.argv[1:]
    if not units:
        cfg = json.load(open(os.path.join(ROOT, "objdiff.json")))
        units = [u["name"] for u in cfg["units"] if u["name"].startswith("hl/")]
    counts = {"clean": 0, "naming": 0, "real": 0}
    real = []
    for unit in units:
        plain = unit_report(unit, [])
        if plain is None:
            continue
        named = unit_report(unit, ["-c", "functionRelocDiffs=name_address"])
        if named is None:
            continue
        for fn, pct in sorted(plain.items()):
            if pct < 100.0:
                continue
            if named.get(fn, 0.0) >= 100.0:
                counts["clean"] += 1
                continue
            found = differences(unit, fn)
            if found:
                counts["real"] += 1
                real.append((unit, fn, sorted(set(found))))
            else:
                counts["naming"] += 1
    for unit, fn, found in real:
        print("%s  %s" % (unit, fn))
        for na, nb in found:
            print("    retail %s" % na)
            print("    ours   %s" % nb)
    print("matched functions: %d clean, %d naming only, %d with real name differences"
          % (counts["clean"], counts["naming"], counts["real"]))


if __name__ == "__main__":
    main()
