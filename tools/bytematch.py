"""Pair retail functions with our compiled functions by identical bytes.

  python tools/bytematch.py [--dll client] [--write] [--at va,va,...] [<unit> ...]

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

--at tests only the given retail addresses (hex, comma separated), with
our function's length, for functions that have no row in progress.csv
(Ghidra merged them into a neighbour).

Prints the proposals; --write stores them in tools/symbols/<dll>_bytes.csv
with their evidence (merged by tools/build.py before <dll>_manual.csv).
Units default to every unit of the dll (hl unless --dll) with both objects.
The address range of a unit runs from the last function of the file
before it to the first function of the file after it, so functions not
yet attributed to any file (client.dll) are candidates too.
"""
import bisect
import csv
import json
import os
import struct
import sys

import pefile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
IMAGES = {"hl": "versions/1.6a/cryoffear/cl_dlls/hl.dll", "client": "versions/1.6a/cryoffear/cl_dlls/client.dll"}
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
    argv = sys.argv[1:]
    dll = "hl"
    if "--dll" in argv:
        i = argv.index("--dll")
        dll = argv[i + 1]
        del argv[i:i + 2]
    at = []
    if "--at" in argv:
        i = argv.index("--at")
        at = [int(x, 16) for x in argv[i + 1].split(",")]
        del argv[i:i + 2]
    args = [a for a in argv if a != "--write"]
    write = "--write" in sys.argv
    cfg = json.load(open(os.path.join(ROOT, "objdiff.json")))
    units = args or [u["name"] for u in cfg["units"] if u["name"].startswith(dll + "/")]
    paths = {u["name"]: u for u in cfg["units"]}
    pe = pefile.PE(os.path.join(ROOT, IMAGES[dll]), fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    # retail functions: address -> (name, size, source file)
    rows = [r for r in csv.DictReader(open(os.path.join(ROOT, "docs", "progress.csv"))) if r["dll"] == dll]
    starts = sorted(int(r["va"], 16) for r in rows)
    names = {}
    for r in csv.DictReader(open(os.path.join(ROOT, "build", "jobs", "symbols_%s.csv" % dll))):
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
        # widen to the neighbouring files: the rows in between belong to no
        # file yet (non-LIB rows only; the CRT sits after all objects)
        src = unit.split("/", 1)[1]
        others = sorted(int(r["va"], 16) for r in rows if r["source_file"] and r["class"] != "LIB"
                        and os.path.splitext(r["source_file"].replace("guess:", ""))[0] != src)
        k = bisect.bisect_left(others, lo)
        lo = others[k - 1] + 1 if k else lo
        k = bisect.bisect_right(others, hi)
        hi = others[k] - 1 if k < len(others) else hi
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
        if at:
            # addresses given by hand (no function row there, for example
            # where Ghidra merged two functions): sizes come from our side
            cands = [(va, names.get(va, ""), None, None) for va in at if lo - 0x10000 <= va <= hi + 0x10000]
        # our candidates: functions whose name has no retail address
        unpaired = {n: f for n, f in funcs.items() if n not in by_name}
        hits = {}
        for va, rn, size, span in cands:
            for n, (code, masked) in unpaired.items():
                if size is not None and len(code) not in (size, span):
                    continue
                ret = img[va - base:va - base + len(code)]
                if all(ret[i] == code[i] for i in range(len(code)) if i not in masked):
                    hits.setdefault(va, []).append(n)
        rev = {}
        for va, ns in hits.items():
            for n in ns:
                rev.setdefault(n, []).append(va)
        for va, ns in sorted(hits.items()):
            # STL instantiations are toolchain code (class LIB) and many are
            # identical to each other
            if len(ns) == 1 and len(rev[ns[0]]) == 1 and "@std@@" not in ns[0]:
                old = names.get(va, "")
                proposals.append((va, ns[0], old, unit))
    for va, n, old, unit in proposals:
        print("%08X  %-60s (now %s)  %s" % (va, n, old or "unnamed", unit))
    print("%d proposals" % len(proposals))
    if write:
        # earlier proposals are named in the merged map now and are not
        # proposed again: keep them and add the new ones
        path = os.path.join(ROOT, "tools", "symbols", "%s_bytes.csv" % dll)
        rows = {}
        if os.path.exists(path):
            for r in csv.DictReader(open(path)):
                rows[int(r["va"], 16)] = (r["name"], r["kind"], r["evidence"])
        for va, n, old, unit in proposals:
            rows[va] = (n, "func", "bytes identical to %s in %s (relocated fields masked), one-to-one%s"
                        % (n, unit, "; was %s" % old if old else ""))
        with open(path, "w", newline="") as f:
            w = csv.writer(f, lineterminator="\n")
            w.writerow(["va", "name", "kind", "evidence"])
            for va in sorted(rows):
                w.writerow(["%08X" % va] + list(rows[va]))
        print("wrote %s (%d entries)" % (os.path.relpath(path, ROOT), len(rows)))


if __name__ == "__main__":
    main()
