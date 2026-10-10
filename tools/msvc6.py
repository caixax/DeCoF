"""Toolchain environment for the VC6 binaries: cl 12.00.8168 / link 6.00.8168.

hw.dll, opengl32.dll, GameUI.dll and cof.exe of 1.6a were built with Visual
C++ 6.0 RTM (Rich header build 8168; opengl32.dll and part of GameUI.dll with
the Standard Edition, which has no optimizer). Files come from the user's
licensed Visual C++ 6.0 Professional Edition (part X04-58170), copied from
the disc's VC98 folder to TOOLS_DIR (no installer run).

Usage as a module: env(), cl_path(), link_path(). As a script:
  python tools/msvc6.py cl <args...>
"""
import os
import subprocess
import sys

ROOT_DIR = r"I:\Tools\vc6\VC98"


def cl_path():
    return os.path.join(ROOT_DIR, "BIN", "CL.EXE")


def link_path():
    return os.path.join(ROOT_DIR, "BIN", "LINK.EXE")


def env():
    e = dict(os.environ)
    e["INCLUDE"] = ";".join([os.path.join(ROOT_DIR, "INCLUDE"), os.path.join(ROOT_DIR, "MFC", "INCLUDE"),
                             os.path.join(ROOT_DIR, "ATL", "INCLUDE")])
    e["LIB"] = ";".join([os.path.join(ROOT_DIR, "LIB"), os.path.join(ROOT_DIR, "MFC", "LIB")])
    e["PATH"] = os.path.join(ROOT_DIR, "BIN") + os.pathsep + e.get("PATH", "")
    for k in ("CL", "_CL_", "LINK", "_LINK_"):
        e.pop(k, None)
    return e


def check():
    for p in (cl_path(), link_path(), os.path.join(ROOT_DIR, "INCLUDE")):
        if not os.path.exists(p):
            sys.exit("missing VC6 component: %s" % p)


if __name__ == "__main__":
    check()
    if len(sys.argv) < 2 or sys.argv[1] not in ("cl", "link"):
        sys.exit("usage: msvc6.py cl|link <args...>")
    exe = cl_path() if sys.argv[1] == "cl" else link_path()
    sys.exit(subprocess.call([exe] + sys.argv[2:], env=env()))
