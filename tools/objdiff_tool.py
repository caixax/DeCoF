"""Helpers around objdiff-cli one-shot JSON diffs.

  python tools/objdiff_tool.py show <unit> <symbol>
      side by side instructions of one function, differing rows marked
  python tools/objdiff_tool.py harvest <unit> [<unit> ...] [--write]
      for every function below 100%: where both sides have a relocation on
      the same aligned instruction and the retail side points at an unnamed
      symbol (DAT_/FUN_/LAB_ + address), take the name of our side as the
      name of that retail address. Prints the proposals; with --write they
      are added to tools/symbols/<dll>_auto.csv (merged into the symbol map
      by tools/map_names.py and used by the next delink).

Left = retail (delinked target), right = our object (base).
"""
import csv
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJDIFF = r"I:\Tools\bin\objdiff-cli.exe"
UNNAMED = re.compile(r"^(?:DAT|FUN|LAB|PTR|s|u|thunk_FUN)_([0-9a-fA-F]{6,8})")


def diff_json(unit, symbol):
    out = os.path.join(ROOT, "build", "diffs", "%s__%s.json" % (unit.replace("/", "_"), re.sub(r"[^\w]", "_", symbol)[:80]))
    os.makedirs(os.path.dirname(out), exist_ok=True)
    subprocess.run([OBJDIFF, "diff", "-p", ROOT, "-u", unit, "-o", out, "--format", "json", symbol],
                   cwd=ROOT, capture_output=True)
    return json.load(open(out))


def sym(side, name):
    for s in side["symbols"]:
        if s.get("name") == name:
            return s
    return None


def rows(d, symbol):
    l, r = sym(d["left"], symbol), sym(d["right"], symbol)
    li = (l or {}).get("instructions") or []
    ri = (r or {}).get("instructions") or []
    return l, r, li, ri


def reloc_name(side, ins):
    rel = (ins.get("instruction") or {}).get("relocation")
    if not rel or "target_symbol" not in rel:
        return None
    idx = int(rel["target_symbol"])
    syms = side["symbols"]
    return syms[idx].get("name") if 0 <= idx < len(syms) else None


def reloc_addend(ins):
    rel = (ins.get("instruction") or {}).get("relocation") or {}
    return int(rel.get("addend", "0"))


def show(unit, symbol):
    d = diff_json(unit, symbol)
    l, r, li, ri = rows(d, symbol)
    print("%s  retail %s%%  ours %s%%" % (symbol, (l or {}).get("match_percent"), (r or {}).get("match_percent")))
    for a, b in zip(li + [{}] * (len(ri) - len(li)), ri + [{}] * (len(li) - len(ri))):
        fa = (a.get("instruction") or {}).get("formatted", "")
        fb = (b.get("instruction") or {}).get("formatted", "")
        mark = " " if not a.get("diff_kind") and not b.get("diff_kind") else "*"
        print("%s %-48s | %-48s %s" % (mark, fa[:48], fb[:48], a.get("diff_kind", "") or b.get("diff_kind", "")))


def report_functions(units):
    rep = json.load(open(os.path.join(ROOT, "build", "report.json")))
    for u in rep.get("units", []):
        if u["name"] in units:
            for fn in u.get("functions", []):
                yield u["name"], fn["name"], fn.get("fuzzy_match_percent", 0.0)


_IMAGE = {}


def retail_bytes(dll, va, n):
    """Bytes of the retail 1.6a image at va (for validating constant names)."""
    import pefile
    paths = {"hl": "versions/1.6a/cryoffear/cl_dlls/hl.dll", "client": "versions/1.6a/cryoffear/cl_dlls/client.dll",
             "opengl32": "versions/1.6a/opengl32.dll", "hw": "versions/1.6a/hw.dll"}
    if dll not in _IMAGE:
        pe = pefile.PE(os.path.join(ROOT, paths[dll]), fast_load=True)
        _IMAGE[dll] = (pe.OPTIONAL_HEADER.ImageBase, pe.get_memory_mapped_image())
    base, img = _IMAGE[dll]
    return img[va - base:va - base + n]


def constant_ok(dll, va, name):
    """__real@<hex> / __xmm@<hex> names encode their value: check it."""
    m = re.match(r"^__(?:real|xmm)@([0-9a-f]+)$", name)
    if not m:
        return True
    hexval = m.group(1)
    want = bytes.fromhex(hexval)[::-1] if name.startswith("__real") else bytes.fromhex(hexval)[::-1]
    return retail_bytes(dll, va, len(want)) == want


def mnemonic(ins):
    for part in (ins.get("instruction") or {}).get("parts", []):
        if "opcode" in part:
            return part["opcode"].get("mnemonic")
    return None


def harvest(units, write, min_pct=80.0):
    found = {}
    conflicts = set()
    for unit, symbol, pct in report_functions(units):
        if pct >= 100.0 or pct < min_pct:
            continue
        d = diff_json(unit, symbol)
        l, r, li, ri = rows(d, symbol)
        if len(li) != len(ri):
            continue
        for a, b in zip(li, ri):
            # only rows where the instruction itself matches and an argument
            # (the relocation) differs
            if a.get("diff_kind") not in (None, "DIFF_ARG_MISMATCH") or mnemonic(a) != mnemonic(b):
                continue
            na, nb = reloc_name(d["left"], a), reloc_name(d["right"], b)
            if not na or not nb or na == nb:
                continue
            m = UNNAMED.match(na)
            if not m or nb.startswith(("$", ".", "[")) or UNNAMED.match(nb):
                continue
            # retail target = unnamed symbol + its addend; our side is
            # name + addend, so the base of our symbol is target - our addend
            va = int(m.group(1), 16) + reloc_addend(a) - reloc_addend(b)
            kind = "func" if na.startswith(("FUN_", "LAB_", "thunk_FUN")) else "data"
            key = (unit.split("/")[0], va)
            if not constant_ok(key[0], va, nb):
                print("value mismatch at %08X for %s in %s (source difference, not a name)" % (va, nb, symbol))
                continue
            prev = found.get(key)
            if prev and prev[0] != nb:
                print("conflict at %08X: %s vs %s (dropped)" % (va, prev[0], nb))
                conflicts.add(key)
                continue
            found[key] = (nb, kind, "objdiff:%s:%s" % (unit, symbol))
    for key in conflicts:
        found.pop(key, None)
    for (dll, va), (name, kind, ev) in sorted(found.items()):
        print("%s %08X %-5s %s" % (dll, va, kind, name))
    if write and found:
        by_dll = {}
        for (dll, va), v in found.items():
            by_dll.setdefault(dll, {})[va] = v
        for dll, items in by_dll.items():
            path = os.path.join(ROOT, "tools", "symbols", "%s_auto.csv" % dll)
            old = {}
            if os.path.exists(path):
                for row in csv.DictReader(open(path)):
                    old[int(row["va"], 16)] = (row["name"], row["kind"], row["evidence"])
            old.update(items)
            with open(path, "w", newline="") as f:
                w = csv.writer(f, lineterminator="\n")
                w.writerow(["va", "name", "kind", "evidence"])
                for va in sorted(old):
                    w.writerow(["%08X" % va] + list(old[va]))
            print("wrote %d entries to %s" % (len(old), os.path.relpath(path, ROOT)))


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    if sys.argv[1] == "show":
        show(sys.argv[2], sys.argv[3])
    elif sys.argv[1] == "harvest":
        args = [a for a in sys.argv[2:] if not a.startswith("--")]
        harvest(args, "--write" in sys.argv)
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
