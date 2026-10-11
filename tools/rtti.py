"""Find MSVC RTTI vtables in a PE image.

  python tools/rtti.py <dll> <class> [slots]
      print the primary vtable of <class>: slot, offset, function address,
      known name (tools/symbols/<dll>_*.csv) and the first instructions

The vtable is found from the type descriptor (".?AV<class>@@"), the
complete object locator that points at it (offset 0) and the pointer to
that locator, which sits right before the vtable.
"""
import csv
import glob
import os
import struct
import sys

import capstone
import pefile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def vtable(pe, img, base, cls):
    td_name = (".?AV%s@@" % cls).encode() + b"\x00"
    pos = img.find(td_name)
    if pos < 0:
        td_name = (".?AU%s@@" % cls).encode() + b"\x00"
        pos = img.find(td_name)
    if pos < 0:
        return None
    td = base + pos - 8
    col = None
    for off in range(0, len(img) - 20, 4):
        sig, o, _cd, ptd = struct.unpack_from("<IIII", img, off)
        if ptd == td and sig == 0 and o == 0:
            col = base + off
            break
    if col is None:
        return None
    ref = img.find(struct.pack("<I", col))
    while ref >= 0 and ref % 4:
        ref = img.find(struct.pack("<I", col), ref + 1)
    return base + ref + 4 if ref >= 0 else None


def main():
    dll, cls = sys.argv[1], sys.argv[2]
    nslots = int(sys.argv[3]) if len(sys.argv) > 3 else 400
    pe = pefile.PE(dll, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    text = [s for s in pe.sections if s.Name.startswith(b".text")][0]
    tlo, thi = base + text.VirtualAddress, base + text.VirtualAddress + text.Misc_VirtualSize
    names = {}
    for path in glob.glob(os.path.join(ROOT, "tools", "symbols", "hl_*.csv")):
        for r in csv.DictReader(open(path)):
            names.setdefault(int(r["va"], 16), r["name"])
    vt = vtable(pe, img, base, cls)
    if vt is None:
        sys.exit("no vtable found for %s" % cls)
    print("vtable of %s at %08X" % (cls, vt))
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    for slot in range(nslots):
        f = struct.unpack_from("<I", img, vt - base + 4 * slot)[0]
        if not tlo <= f < thi:
            break
        code = img[f - base:f - base + 32]
        ins = "; ".join("%s %s" % (i.mnemonic, i.op_str) for i in list(md.disasm(code, f))[:5])
        print("%3d +0x%03X %08X %-60s %s" % (slot, slot * 4, f, names.get(f, "")[:60], ins))


if __name__ == "__main__":
    main()
