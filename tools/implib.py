"""Make an import library for a retail third-party DLL from its export table.

  python tools/implib.py <retail.dll> <out.lib>

Out-of-scope DLLs that the game DLLs import from (steam_api.dll) ship
without their .lib. This writes a .def file with every exported name of the
retail DLL, in the DLL's own order with its ordinals, and runs lib.exe of
the target toolchain on it. Nothing of the DLL's code is used.
"""
import os
import subprocess
import sys

import pefile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import msvc  # noqa: E402


def main():
    dll, out = sys.argv[1], sys.argv[2]
    pe = pefile.PE(dll, fast_load=True)
    pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_EXPORT"]])
    name = os.path.basename(dll)
    lines = ["LIBRARY %s" % name, "EXPORTS"]
    for s in pe.DIRECTORY_ENTRY_EXPORT.symbols:
        if s.name:
            lines.append("\t%s @%d" % (s.name.decode(), s.ordinal))
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
