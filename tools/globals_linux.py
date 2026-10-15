"""Name the globals of a VC6 engine DLL through the Linux build's relocations.

  python tools/globals_linux.py <dll> <lib.so> [--write]        (dll: hw, gameui)

The Linux steam_legacy libraries are linked with text relocations: every
instruction that touches a global keeps an R_386_32 relocation naming the
symbol (plus an addend, the member offset). The Windows DLL's base
relocations mark every absolute address in its code. For each function
known on both sides (a Windows row whose name is the Linux name, C names
with the leading underscore), this takes the distinct data symbols the
Linux function references with addend 0, in order of first use, and the
distinct data addresses the Windows function references, in order of first
use. Pairs of equal length vote (windows address -> linux symbol); a
symbol wins an address when it has at least two votes and four in five of
the votes for that address. Prints the result; --write records them in
tools/symbols/<dll>_manual.csv (kind data, with the vote as evidence)
unless the address is named already. C++ names (_Z...) and GCC static
locals (name.N) are printed only, they have no MSVC name to write.
"""
import bisect
import collections
import csv
import os
import struct
import sys

import pefile
from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import name_unit  # noqa: E402

IMAGES = {"hw": "versions/1.6a/hw.dll", "gameui": "versions/1.6a/cryoffear/cl_dlls/GameUI.dll"}


def linux_refs(lib):
    """linux function name -> [(symbol or None, offset)] in code order.

    R_386_32 relocations name exported data (the in-place value is the
    member offset). R_386_RELATIVE relocations hold the link-time address
    of static data: it is resolved against the .symtab objects, and an
    address inside no object (a string literal or a float constant) gives
    (None, address), a slot that takes no vote."""
    elf = ELFFile(open(lib, "rb"))
    symtab = elf.get_section_by_name(".symtab")
    funcs, objs = [], []
    for s in symtab.iter_symbols():
        t = s["st_info"]["type"]
        if t == "STT_FUNC" and s["st_value"] and s["st_size"]:
            funcs.append((s["st_value"], s["st_size"], s.name))
        elif t == "STT_OBJECT" and s["st_value"]:
            objs.append((s["st_value"], max(s["st_size"], 1), s.name))
    funcs.sort()
    objs.sort()
    starts = [f[0] for f in funcs]
    ostarts = [o[0] for o in objs]
    text = elf.get_section_by_name(".text")
    tdata = text.data()
    tbase = text["sh_addr"]
    refs = collections.defaultdict(list)
    for sec in elf.iter_sections():
        if not isinstance(sec, RelocationSection) or sec.name != ".rel.dyn":
            continue
        st = elf.get_section(sec["sh_link"])
        for rel in sec.iter_relocations():
            off = rel["r_offset"]
            if not tbase <= off < tbase + len(tdata):
                continue
            val = struct.unpack_from("<I", tdata, off - tbase)[0]
            kind = rel["r_info_type"]
            if kind == 1 and rel["r_info_sym"]:  # R_386_32
                sym = st.get_symbol(rel["r_info_sym"])
                if sym["st_info"]["type"] != "STT_OBJECT":
                    continue
                item = (sym.name, val)
            elif kind == 8:  # R_386_RELATIVE
                if tbase <= val < tbase + len(tdata):
                    continue  # code address (jump table, function pointer)
                j = bisect.bisect_right(ostarts, val) - 1
                if j >= 0 and val < objs[j][0] + objs[j][1]:
                    item = (objs[j][2], val - objs[j][0])
                else:
                    item = (None, val)
            else:
                continue
            i = bisect.bisect_right(starts, off) - 1
            if i >= 0 and off < funcs[i][0] + funcs[i][1]:
                refs[funcs[i][2]].append((off, item))
    for n in refs:
        refs[n] = [it for o, it in sorted(refs[n])]
    return refs


def windows_refs(dll, rows):
    pe = pefile.PE(os.path.join(ROOT, IMAGES[dll]))
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    text = [s for s in pe.sections if s.Name.startswith(b".text")][0]
    tlo, thi = base + text.VirtualAddress, base + text.VirtualAddress + text.Misc_VirtualSize
    relocs = []
    for blk in getattr(pe, "DIRECTORY_ENTRY_BASERELOC", []):
        for e in blk.entries:
            if e.type == 3:
                relocs.append(base + e.rva)
    relocs.sort()
    out = {}
    for va, size, name in rows:
        i = bisect.bisect_left(relocs, va)
        lst = []
        while i < len(relocs) and relocs[i] < va + size:
            tgt = struct.unpack_from("<I", img, relocs[i] - base)[0]
            if not tlo <= tgt < thi:
                lst.append(tgt)
            i += 1
        out[name] = lst
    return out


def distinct(seq):
    seen, out = set(), []
    for x in seq:
        if x not in seen:
            seen.add(x)
            out.append(x)
    return out


def main():
    dll, lib = sys.argv[1], sys.argv[2]
    write = "--write" in sys.argv
    lrefs = linux_refs(lib)
    rows = []
    for r in csv.DictReader(open(os.path.join(ROOT, "docs", "progress.csv"), encoding="utf-8")):
        if r["dll"] == dll and r["name"] and r["class"] != "LIB":
            n = r["name"][1:] if r["name"].startswith("_") and not r["name"].startswith("__") else r["name"]
            if n in lrefs:
                rows.append((int(r["va"], 16), int(r["size"]), n))
    wrefs = windows_refs(dll, rows)
    final = {}  # windows address -> (symbol, votes, total)
    known = {}  # symbol -> windows address
    for _ in range(4):
        votes = collections.defaultdict(collections.Counter)
        pairs = 0
        for va, size, n in rows:
            L = distinct(lrefs[n])
            W = distinct(wrefs.get(n, []))
            gone = {known[s] + a for s, a in L if s in known}
            L = [(s, a) for s, a in L if s not in known]
            W = [w for w in W if w not in gone]
            if any(s for s, a in L) and len(L) == len(W):
                pairs += 1
                for w, (s, a) in zip(W, L):
                    if s:
                        votes[(w - a) & 0xFFFFFFFF][s] += 1
        named = {}
        for w, c in votes.items():
            if w in final:
                continue
            s, k = c.most_common(1)[0]
            if k >= 2 and k * 5 >= sum(c.values()) * 4:
                named[w] = (s, k, sum(c.values()))
        bysym = collections.defaultdict(list)
        for w, (s, k, t) in named.items():
            bysym[s].append((k, w))
        new = 0
        for s, lst in bysym.items():
            if s in known:
                continue
            lst.sort(reverse=True)
            if len(lst) == 1 or lst[0][0] >= 2 * lst[1][0]:
                k, w = lst[0]
                final[w] = named[w]
                known[s] = w
                new += 1
        if not new:
            break
    print("%d function pairs voted, %d addresses named" % (pairs, len(final)))
    for w, (s, k, t) in sorted(final.items())[:None if "--all" in sys.argv else 12]:
        print("  %08X %-32s %d/%d" % (w, s, k, t))
    if not write:
        return
    name_unit.lock()
    try:
        p = os.path.join(ROOT, "tools", "symbols", "%s_manual.csv" % dll)
        old = list(csv.DictReader(open(p))) if os.path.exists(p) else []
        have = {r["va"] for r in old}
        n = 0
        for w, (s, k, t) in sorted(final.items()):
            va = "%08X" % w
            if va in have or s.startswith("_Z") or "." in s:
                continue  # C++ names need the MSVC decoration, GCC static locals have none
            old.append({"va": va, "name": "_" + s, "kind": "data",
                        "evidence": "Linux relocations of %d of %d paired functions name it %s (tools/globals_linux.py)" % (k, t, s)})
            n += 1
        with open(p, "w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=["va", "name", "kind", "evidence"], lineterminator="\n")
            w.writeheader()
            w.writerows(old)
        print("recorded %d" % n)
    finally:
        name_unit.unlock()


if __name__ == "__main__":
    main()
