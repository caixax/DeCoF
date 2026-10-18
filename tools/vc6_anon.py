"""Give the anonymous namespaces of a VC6 object their retail names.

  python tools/vc6_anon.py <obj> [<obj> ...]

tools/build.py runs it on every VC6 object it compiles.

VC6 names an anonymous namespace ?%<source path><number>@@ where <number>
changes with every compile (it is not derived from the source), so the
decorated names of everything inside such a namespace change on each
build. This rewrites the symbol names of the object so that each
?%<path>/<File>.cpp<number>@@ becomes the name the retail binary's RTTI
records for that file (table below), and leaves the code untouched.
"""
import re
import struct
import sys

# file name and number of the anonymous namespace in the retail RTTI type
# descriptors (tools/rtti.py); the directory is dropped because Ghidra symbol
# names cannot hold the spaces of C:\Documents and Settings\...
RETAIL = {
    "frame.cpp": "Frame.cpp20302634",
    "scrollbar.cpp": "ScrollBar.cpp989430212",
}

ANON = re.compile(rb"\?%([^@]*?)([^\\/@]+\.cpp)(\d+)@@")


def fix_name(name):
    def rep(m):
        key = m.group(2).decode("latin1").lower()
        if key not in RETAIL:
            return m.group(0)
        return b"?%" + RETAIL[key].encode("latin1") + b"@@"
    return ANON.sub(rep, name)


def fix(path):
    data = bytearray(open(path, "rb").read())
    _m, nsec, _ts, symptr, nsym, opt, _ch = struct.unpack_from("<HHIIIHH", data, 0)
    strtab = symptr + nsym * 18
    strsize = struct.unpack_from("<I", data, strtab)[0]
    old = bytes(data[strtab:strtab + strsize])
    names = []
    i = 0
    while i < nsym:
        o = symptr + i * 18
        raw = bytes(data[o:o + 8])
        if raw[:4] == b"\0\0\0\0":
            off = struct.unpack_from("<I", raw, 4)[0]
            e = old.index(b"\0", off)
            names.append((o, old[off:e], True))
        else:
            names.append((o, raw.rstrip(b"\0"), False))
        naux = data[o + 17]
        i += 1 + naux
    changed = 0
    # keep the old table (section headers may point into it) and append
    new = bytearray(old)
    for o, name, is_long in names:
        fixed = fix_name(name)
        if fixed == name:
            continue
        changed += 1
        if len(fixed) > 8:
            struct.pack_into("<II", data, o, 0, len(new))
            new += fixed + b"\0"
        else:
            data[o:o + 8] = fixed.ljust(8, b"\0")
    if not changed:
        return 0
    struct.pack_into("<I", new, 0, len(new))
    data = data[:strtab] + new
    open(path, "wb").write(data)
    return changed


if __name__ == "__main__":
    for p in sys.argv[1:]:
        print("%s: %d names" % (p, fix(p)))
