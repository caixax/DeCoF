"""Disassemble retail code with names and strings.

  python tools/disasm.py <dll> <va|decorated name> [bytes]

Prints the instructions of a function (or <bytes> bytes from <va>) with
the symbol names of call/jump targets and data from the merged symbol map
(build/jobs/symbols_<dll>.csv, else tools/symbols/<dll>_*.csv) and the
text of referenced strings. Stops at the next function start when no
length is given.
"""
import csv
import glob
import os
import re
import sys

import capstone
import pefile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
IMAGES = {"hl": "versions/1.6a/cryoffear/cl_dlls/hl.dll", "client": "versions/1.6a/cryoffear/cl_dlls/client.dll",
          "hw": "versions/1.6a/hw.dll", "opengl32": "versions/1.6a/opengl32.dll"}


def main():
    dll, what = sys.argv[1], sys.argv[2]
    length = int(sys.argv[3], 0) if len(sys.argv) > 3 else None
    pe = pefile.PE(os.path.join(ROOT, IMAGES[dll]), fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    names = {}
    paths = [os.path.join(ROOT, "build", "jobs", "symbols_%s.csv" % dll)]
    if not os.path.exists(paths[0]):
        paths = glob.glob(os.path.join(ROOT, "tools", "symbols", "%s_*.csv" % dll))
    for p in paths:
        for r in csv.DictReader(open(p)):
            if "va" in r and "name" in r:
                names.setdefault(int(r["va"], 16), r["name"])
    sizes = {}
    for r in csv.DictReader(open(os.path.join(ROOT, "docs", "progress.csv"))):
        if r["dll"] == dll:
            sizes[int(r["va"], 16)] = int(r["size"] or 0)
            if r["name"]:
                names.setdefault(int(r["va"], 16), r["name"])
    if re.match(r"^(0x)?[0-9a-fA-F]{8}$", what):
        va = int(what, 16)
    else:
        va = next((a for a, n in names.items() if n == what), None)
        if va is None:
            sys.exit("unknown name %s" % what)
    if length is None:
        length = sizes.get(va) or 0x200

    def cs(a):
        o = a - base
        if 0 <= o < len(img):
            e = img.find(b"\0", o, o + 100)
            s = img[o:e]
            if e > o + 1 and all(32 <= c < 127 for c in s):
                return s.decode()
        return None

    # named globals with a size: references into them print as name+offset
    sizes_by_name = {}
    sp = os.path.join(ROOT, "tools", "symbols", "%s_datasizes.csv" % dll)
    if os.path.exists(sp):
        sizes_by_name = {r["name"]: int(r["size"]) for r in csv.DictReader(open(sp))}
    spans = sorted((a, a + sizes_by_name[n], n) for a, n in names.items() if n in sizes_by_name)
    starts = [s[0] for s in spans]
    members = {}
    lp = os.path.join(ROOT, "build", "types", "layout_enginefuncs.txt")
    if os.path.exists(lp):
        for line in open(lp):
            p = line.split()
            members[("?g_engfuncs@@3Uenginefuncs_s@@A", int(p[0], 16))] = p[-1]

    def inside(a):
        import bisect
        i = bisect.bisect_right(starts, a) - 1
        if i >= 0 and spans[i][0] < a < spans[i][1]:
            off = a - spans[i][0]
            m = members.get((spans[i][2], off))
            return "%s+0x%X%s" % (spans[i][2], off, " (%s)" % m if m else "")
        return None

    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    print("%08X %s" % (va, names.get(va, "")))
    for ins in md.disasm(img[va - base:va - base + length], va):
        notes = []
        for tok in re.findall(r"0x[0-9a-f]+", ins.op_str):
            a = int(tok, 16)
            if a in names and a != va:
                notes.append(names[a])
            else:
                s = cs(a)
                if s:
                    notes.append('"%s"' % s)
                else:
                    n = inside(a)
                    if n:
                        notes.append(n)
        print("  %08X  %-40s %s" % (ins.address, (ins.mnemonic + " " + ins.op_str)[:40],
                                     ("; " + ", ".join(notes)) if notes else ""))


if __name__ == "__main__":
    main()
