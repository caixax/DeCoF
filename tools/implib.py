"""Make an import library for a retail third-party DLL from its export table.

  python tools/implib.py <retail.dll> <out.lib> [<object dir>]

Out-of-scope DLLs that the game DLLs import from (steam_api.dll) ship
without their .lib. This writes a .def file with every exported name of the
retail DLL, by name in the DLL's own order, and runs lib.exe of
the target toolchain on it. Nothing of the DLL's code is used.

__stdcall functions are exported undecorated (DevIL's ilInit) but imported
as __imp__ilInit@0: with an object directory, every __imp__<name>@<n> that
our objects reference is written as <name>@<n>, which lib.exe turns back
into the undecorated import name.
"""
import glob
import os
import re
import subprocess
import sys

import pefile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import msvc  # noqa: E402


def main():
    dll, out = sys.argv[1], sys.argv[2]
    pe = pefile.PE(dll, fast_load=True)
    pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_EXPORT"]])
    # the name the DLL gives itself (DevIL.dll), which importers record,
    # rather than the file name on disk (devil.dll)
    name = pe.get_string_at_rva(pe.DIRECTORY_ENTRY_EXPORT.struct.Name).decode() or os.path.basename(dll)
    stdcall = {}
    if len(sys.argv) > 3:
        for obj in glob.glob(os.path.join(sys.argv[3], "**", "*.obj"), recursive=True):
            for m in re.findall(rb"__imp__([A-Za-z_][A-Za-z0-9_]*)@([0-9]+)", open(obj, "rb").read()):
                stdcall[m[0].decode()] = m[1].decode()
    lines = ["LIBRARY %s" % name, "EXPORTS"]
    for s in pe.DIRECTORY_ENTRY_EXPORT.symbols:
        if s.name:
            n = s.name.decode()
            lines.append("\t%s@%s" % (n, stdcall[n]) if n in stdcall else "\t%s" % n)
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    deffile = os.path.splitext(out)[0] + ".def"
    with open(deffile, "w") as f:
        f.write("\n".join(lines) + "\n")
    tools = os.path.dirname(msvc.link_path())
    rc = subprocess.call([os.path.join(tools, "lib.exe"), "/nologo", "/machine:x86", "/def:" + deffile,
                          "/out:" + out], env=msvc.env(), stdout=subprocess.DEVNULL)
    if rc != 0:
        sys.exit("lib.exe failed (%d)" % rc)
    print("%s: %d exports -> %s" % (name, len(lines) - 2, out))


if __name__ == "__main__":
    main()
