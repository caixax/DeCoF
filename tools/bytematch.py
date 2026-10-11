"""Pair retail functions with our compiled functions by identical bytes.

  python tools/bytematch.py [--write] [<unit> ...]

A retail function that is unnamed in the symbol map (or carries a name
that no object of ours defines) never meets its source counterpart in
objdiff. For every such retail function inside the address range of a
unit, this compares its bytes with every function of our compiled object
for that unit that has no retail counterpart either. Relocated fields are
masked (our relocation offsets, applied to both sides, so the
instructions must be identical); the sizes must be equal (for a function
whose COMDAT also holds jump tables, the retail range runs to the next
function). Only one-to-one matches are proposed: one retail function,
one function of ours, nothing else identical.

Prints the proposals; --write stores them in tools/symbols/hl_bytes.csv
with their evidence (merged by tools/build.py before hl_manual.csv).
Units default to every hl unit with both objects.
"""
import bisect
import csv
import json
import os
import struct
import sys

import pefile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
IMAGE = os.path.join(ROOT, "versions/1.6a/cryoffear/cl_dlls/hl.dll")
DEFAULT = ("FUN_", "LAB_", "thunk_", "SUB_")


def coff_functions(path):
    """{name: (bytes, set of masked offsets)} for the function symbols of a
    COFF object compiled with /Gy (one COMDAT section per function)."""
    data = open(path, "rb").read()
    _m, nsec, _ts, symptr, nsym, opt, _ch = struct.unpack_from("<HHIIIHH", data, 0)
    secs = []
    for i in range(nsec):
        o = 20 + opt + 40 * i
        size, praw, prel, _pl, nrel, _nl, chars = struct.unpack_from("<IIIIHHI", data, o + 16)
        raw = data[praw:praw + size] if praw else b""
        masked = set()
        for k in range(nrel):
            va, _si, typ = struct.unpack_from("<IIH", data, prel + 10 * k)
            masked.update(range(va, va + 4))
        secs.append((raw, masked, chars))
    strtab = symptr + 18 * nsym
    out = {}
    i = 0
    while i < nsym:
        o = symptr + 18 * i
        raw = data[o:o + 8]
        if raw[:4] == b"\0\0\0\0":
            off = struct.unpack_from("<I", raw, 4)[0]
            name = data[strtab + off:data.index(b"\0", strtab + off)].decode("latin-1")
        else:
            name = raw.rstrip(b"\0").decode("latin-1")
        value, secnum, typ, cls, naux = struct.unpack_from("<IhHBB", data, o + 8)
        if secnum > 0 and typ == 0x20 and cls == 2 and value == 0:
            code, masked, chars = secs[secnum - 1]
            if chars & 0x20:  # IMAGE_SCN_CNT_CODE
                out[name] = (code, masked)
        i += 1 + naux
    return out


def main():
    args = [a for a in sys.argv[1:] if a != "--write"]
    write = "--write" in sys.argv
    cfg = json.load(open(os.path.join(ROOT, "objdiff.json")))
    units = args or [u["name"] for u in cfg["units"] if u["name"].startswith("hl/")]
    paths = {u["name"]: u for u in cfg["units"]}
    pe = pefile.PE(IMAGE, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    # retail functions: address -> (name, size, source file)
    rows = [r for r in csv.DictReader(open(os.path.join(ROOT, "docs", "progress.csv"))) if r["dll"] == "hl"]
    starts = sorted(int(r["va"], 16) for r in rows)
    names = {}
    for r in csv.DictReader(open(os.path.join(ROOT, "build", "jobs", "symbols_hl.csv"))):
        names[int(r["va"], 16)] = r["name"]
    by_name = {}
    for va, n in names.items():
        by_name.setdefault(n, []).append(va)
    ours_all = {}
    for unit in units:
        u = paths.get(unit)
        if not u or not os.path.exists(os.path.join(ROOT, u["base_path"])):
            continue
        ours_all[unit] = coff_functions(os.path.join(ROOT, u["base_path"]))
    defined = set()
    for funcs in ours_all.values():
        defined.update(funcs)
    proposals = []
    for unit, funcs in ours_all.items():
        mine = [r for r in rows if r["source_file"].replace("guess:", "") == unit.split("/", 1)[1] + ".cpp"
                or r["source_file"].replace("guess:", "") == unit.split("/", 1)[1] + ".c"]
        if not mine:
            continue
        lo = min(int(r["va"], 16) for r in mine)
        hi = max(int(r["va"], 16) for r in mine)
        # retail candidates: unpaired functions inside the unit's range
        cands = []
        for r in rows:
            va = int(r["va"], 16)
            if not lo <= va <= hi or not r["size"]:
                continue
            n = names.get(va, "")
            if n and not n.startswith(DEFAULT) and n in defined:
                continue
            k = bisect.bisect_right(starts, va)
            end = starts[k] if k < len(starts) else va + int(r["size"])
            cands.append((va, n, int(r["size"]), end - va))
        # our candidates: functions whose name has no retail address
        unpaired = {n: f for n, f in funcs.items() if n not in by_name}
        hits = {}
        for va, rn, size, span in cands:
            for n, (code, masked) in unpaired.items():
                if len(code) not in (size, span):
                    continue
                ret = img[va - base:va - base + len(code)]
                if all(ret[i] == code[i] for i in range(len(code)) if i not in masked):
                    hits.setdefault(va, []).append(n)
        rev = {}
        for va, ns in hits.items():
            for n in ns:
                rev.setdefault(n, []).append(va)
        for va, ns in sorted(hits.items()):
            if len(ns) == 1 and len(rev[ns[0]]) == 1:
                old = names.get(va, "")
                proposals.append((va, ns[0], old, unit))
    for va, n, old, unit in proposals:
        print("%08X  %-60s (now %s)  %s" % (va, n, old or "unnamed", unit))
    print("%d proposals" % len(proposals))
    if write:
        path = os.path.join(ROOT, "tools", "symbols", "hl_bytes.csv")
        with open(path, "w", newline="") as f:
            w = csv.writer(f, lineterminator="\n")
            w.writerow(["va", "name", "kind", "evidence"])
            for va, n, old, unit in proposals:
                w.writerow(["%08X" % va, n, "func", "bytes identical to %s in %s (relocated fields masked), one-to-one%s"
                            % (n, unit, "; was %s" % old if old else "")])
        print("wrote %s" % os.path.relpath(path, ROOT))


if __name__ == "__main__":
    main()
