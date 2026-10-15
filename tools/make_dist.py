"""Assemble a playable game in dist/ from retail 1.6a plus rebuilt DLLs.

Usage: python tools/make_dist.py [--dry-run] [--keep] [--bin build/bin]

1. Deletes dist/ and copies versions/1.6a into it (files are copied, never
   hard linked, so the game can not write through into versions/).
2. For every in-scope DLL, if the build produced <bin>/<name> the retail
   copy in dist/ is replaced. The build only writes a DLL to <bin> when it
   links completely from src/, so a missing file means "keep retail".

--keep leaves an existing dist/ in place and only sets every in-scope DLL
to its rebuilt or retail copy (for example --keep --bin <empty dir> puts
the retail DLLs back for a comparison run).
"""
import argparse
import hashlib
import os
import shutil
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RETAIL = os.path.join(ROOT, "versions", "1.6a")
DIST = os.path.join(ROOT, "dist")

# Rebuilt file name -> location inside the game tree
DLLS = {
    "client.dll": os.path.join("cryoffear", "cl_dlls", "client.dll"),
    "hl.dll": os.path.join("cryoffear", "cl_dlls", "hl.dll"),
    "opengl32.dll": "opengl32.dll",
    "CoFLaunchApp.exe": "CoFLaunchApp.exe",
}


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--bin", default=os.path.join(ROOT, "build", "bin"))
    ap.add_argument("--keep", action="store_true")
    args = ap.parse_args()

    if not os.path.isdir(RETAIL):
        sys.exit("retail tree not found: %s" % RETAIL)
    if os.path.normcase(os.path.dirname(DIST)) != os.path.normcase(ROOT):
        sys.exit("refusing to touch %s" % DIST)

    replace = []
    for name, rel in sorted(DLLS.items()):
        built = os.path.join(args.bin, name)
        if os.path.isfile(built):
            replace.append((name, built, rel))
            print("rebuilt  %-13s <- %s" % (name, built))
        else:
            print("retail   %-13s (no complete build in %s)" % (name, args.bin))

    if args.dry_run:
        print("dry run: would delete %s, copy %s, then replace %d DLL(s)"
              % (DIST, RETAIL, len(replace)))
        return

    if args.keep and os.path.isdir(DIST):
        for name, rel in sorted(DLLS.items()):
            shutil.copyfile(os.path.join(RETAIL, rel), os.path.join(DIST, rel))
    else:
        if os.path.exists(DIST):
            shutil.rmtree(DIST)
        shutil.copytree(RETAIL, DIST)
    for name, built, rel in replace:
        dest = os.path.join(DIST, rel)
        shutil.copyfile(built, dest)
        print("replaced %s (sha256 %s)" % (rel, sha256(dest)))
    print("dist ready: %s" % DIST)


if __name__ == "__main__":
    main()
