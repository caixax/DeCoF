"""Progress report: docs/progress.json, docs/progress.svg, docs/PROGRESS.md.

  python tools/progress.py [--csv PATH]

Reads docs/progress.csv (or PATH, used by the pre-commit hook to read the
staged copy) and writes the three outputs. Standard library only. Every
number comes from the CSV; never edit the outputs by hand.

Counting rules (also written into PROGRESS.md):
- Rows of class LIB (static CRT, STL and compiler helpers from the
  toolchain) are not decompiled and are left out of every figure.
- MATCHED: objdiff reports 100 percent for the function.
- EQUIVALENT: compiles, logic verified against the assembly, not byte
  identical.
- TODO: everything else.
- Byte figures weight each function by its size in the retail binary
  (column "size").
"""
import argparse
import csv
import datetime
import json
import os
import subprocess

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DOCS = os.path.join(ROOT, "docs")

# CSV dll key -> binary name, in report order
BINARIES = [
    ("client", "client.dll"),
    ("hl", "hl.dll"),
    ("opengl32", "opengl32.dll"),
    ("hw", "hw.dll"),
    ("gameui", "GameUI.dll"),
    ("launcher", "cof.exe"),
    ("coflaunchapp", "CoFLaunchApp.exe"),
]
# binaries given a category table in PROGRESS.md and counted in "shown_total"
SHOWN = ["client", "hl", "opengl32", "coflaunchapp"]
# why a binary has not started, drawn next to its bar in the SVG
NOTES = {
    "hw": "not started (VC6 installed; processor pack and VS 2005 objects need those compilers)",
    "gameui": "not started (VC6 installed)",
    "launcher": "not started (VC6 installed)",
}
# in scope but not in the CSV yet: (name, note)
NOT_INVENTORIED = []
CATEGORIES = ["HLSDK", "PARANOIA", "SPIRIT", "COF", "UNKNOWN"]
STATES = ["MATCHED", "EQUIVALENT", "TODO"]

COLORS = {"MATCHED": "#2da44e", "EQUIVALENT": "#d4a72c", "TODO": "#afb8c1"}


def empty():
    return {"functions": {"total": 0, "MATCHED": 0, "EQUIVALENT": 0, "TODO": 0},
            "bytes": {"total": 0, "MATCHED": 0, "EQUIVALENT": 0, "TODO": 0}}


def add(acc, state, size):
    for key, n in (("functions", 1), ("bytes", size)):
        acc[key]["total"] += n
        acc[key][state] += n


def merge(dst, src):
    for key in ("functions", "bytes"):
        for k, v in src[key].items():
            dst[key][k] += v


def pct(part, whole):
    return 100.0 * part / whole if whole else 0.0


def git(*args):
    try:
        return subprocess.run(["git"] + list(args), cwd=ROOT, capture_output=True,
                              text=True, check=True).stdout.strip()
    except (OSError, subprocess.CalledProcessError):
        return ""


def target_version():
    """Version of the target build, from docs/versions.csv (the row whose
    note marks it as the target; its version is the version string found
    in client.dll, see docs/sessions.md)."""
    path = os.path.join(DOCS, "versions.csv")
    if os.path.exists(path):
        for r in csv.DictReader(open(path, encoding="utf-8")):
            if (r.get("note") or "").startswith("target"):
                return r["version"]
    return "unknown"


def collect(csv_path):
    per = {key: empty() for key, _ in BINARIES}
    cats = {key: {} for key, _ in BINARIES}
    for r in csv.DictReader(open(csv_path, encoding="utf-8")):
        dll, cls = r["dll"], r["class"]
        if dll not in per or cls == "LIB":
            continue
        state = r["status"] if r["status"] in ("MATCHED", "EQUIVALENT") else "TODO"
        size = int(r["size"] or 0)
        add(per[dll], state, size)
        add(cats[dll].setdefault(cls or "UNKNOWN", empty()), state, size)
    return per, cats


def write_json(per, cats, meta):
    out = {"target_version": meta["version"], "commit": meta["commit"], "date": meta["date"],
           "counting": "LIB rows excluded; bytes weighted by function size",
           "dlls": {}, "total": empty(), "shown_total": empty()}
    for key, name in BINARIES:
        d = dict(per[key])
        d["categories"] = {c: cats[key][c] for c in sorted(cats[key])}
        out["dlls"][name] = d
        merge(out["total"], per[key])
        if key in SHOWN:
            merge(out["shown_total"], per[key])
    with open(os.path.join(DOCS, "progress.json"), "w", newline="\n") as f:
        json.dump(out, f, indent=2, sort_keys=True)
        f.write("\n")
    return out


def esc(s):
    return s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def write_svg(per, meta):
    width = 760
    pad = 24
    bar_w = width - 2 * pad
    rows = [(name, per[key], NOTES.get(key)) for key, name in BINARIES]
    rows += [(name, None, note) for name, note in NOT_INVENTORIED]
    shown = empty()
    total = empty()
    for key, name in BINARIES:
        merge(total, per[key])
        if key in SHOWN:
            merge(shown, per[key])
    rows.append(("Started binaries (%s)" % ", ".join(n for k, n in BINARIES if k in SHOWN), shown, None))
    rows.append(("All binaries (CoFLaunchApp.exe not counted yet)", total, None))

    row_h = 74
    top = 78
    height = top + row_h * len(rows) + 28
    font = "font-family=\"Segoe UI, Helvetica, Arial, sans-serif\""
    o = []
    o.append('<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" viewBox="0 0 %d %d" role="img" '
             'aria-label="Cry of Fear decompilation progress">' % (width, height, width, height))
    # own card background, so the text reads the same on light and dark pages
    o.append('<rect x="0.5" y="0.5" width="%d" height="%d" rx="10" fill="#ffffff" stroke="#d0d7de"/>'
             % (width - 1, height - 1))
    o.append('<text x="%d" y="34" %s font-size="20" font-weight="600" fill="#1f2328">'
             'Cry of Fear decompilation progress</text>' % (pad, font))
    # legend
    lx = pad
    for state in STATES:
        o.append('<rect x="%d" y="48" width="12" height="12" rx="2" fill="%s"/>' % (lx, COLORS[state]))
        o.append('<text x="%d" y="58.5" %s font-size="12" fill="#1f2328">%s</text>' % (lx + 17, font, state))
        lx += 17 + 8 * len(state) + 26
    o.append('<text x="%d" y="58.5" %s font-size="12" fill="#57606a" text-anchor="end">'
             'percent of code bytes, function counts in brackets</text>' % (width - pad, font))
    y = top
    for name, d, note in rows:
        if d is None:
            # in scope, no rows in the CSV yet
            o.append('<text x="%d" y="%d" %s font-size="14" font-weight="600" fill="#1f2328">%s</text>'
                     % (pad, y + 14, font, esc(name)))
            o.append('<text x="%d" y="%d" %s font-size="14" font-weight="600" fill="#1f2328" text-anchor="end">'
                     'not started</text>' % (width - pad, y + 14, font))
            o.append('<rect x="%d" y="%d" width="%d" height="18" rx="4" fill="%s"/>'
                     % (pad, y + 22, bar_w, COLORS["TODO"]))
            o.append('<text x="%d" y="%d" %s font-size="12" fill="#57606a">%s</text>'
                     % (pad, y + 56, font, esc(note)))
            y += row_h
            continue
        b, fn = d["bytes"], d["functions"]
        done = pct(b["MATCHED"] + b["EQUIVALENT"], b["total"])
        o.append('<text x="%d" y="%d" %s font-size="14" font-weight="600" fill="#1f2328">%s</text>'
                 % (pad, y + 14, font, esc(name)))
        o.append('<text x="%d" y="%d" %s font-size="14" font-weight="600" fill="#1f2328" text-anchor="end">'
                 '%.1f%% done (%d of %d functions)</text>'
                 % (width - pad, y + 14, font, done, fn["MATCHED"] + fn["EQUIVALENT"], fn["total"]))
        by = y + 22
        o.append('<rect x="%d" y="%d" width="%d" height="18" rx="4" fill="%s"/>' % (pad, by, bar_w, COLORS["TODO"]))
        x = float(pad)
        for state in ("MATCHED", "EQUIVALENT"):
            w = bar_w * (b[state] / b["total"]) if b["total"] else 0.0
            if w > 0:
                o.append('<rect x="%.2f" y="%d" width="%.2f" height="18" fill="%s"/>' % (x, by, w, COLORS[state]))
            x += w
        detail = "MATCHED %.1f%% (%d)   EQUIVALENT %.1f%% (%d)   TODO %.1f%% (%d)   %d code bytes" % (
            pct(b["MATCHED"], b["total"]), fn["MATCHED"], pct(b["EQUIVALENT"], b["total"]), fn["EQUIVALENT"],
            pct(b["TODO"], b["total"]), fn["TODO"], b["total"])
        if note:
            detail += "   (%s)" % note
        o.append('<text x="%d" y="%d" %s font-size="12" fill="#57606a" xml:space="preserve">%s</text>'
                 % (pad, by + 34, font, esc(detail)))
        y += row_h
    foot = "Target build %s | commit %s | %s | numbers from docs/progress.csv" % (
        meta["version"], meta["commit"] or "unknown", meta["date"] or "unknown")
    o.append('<text x="%d" y="%d" %s font-size="11" fill="#57606a">%s</text>'
             % (pad, height - 16, font, esc(foot)))
    o.append("</svg>")
    with open(os.path.join(DOCS, "progress.svg"), "w", newline="\n") as f:
        f.write("\n".join(o) + "\n")


def write_md(per, cats, meta):
    o = ["# Progress", "",
         "Generated by `tools/progress.py` from `docs/progress.csv`. Do not edit by hand.", "",
         "![progress](progress.svg)", "",
         "Target build %s, commit %s, %s." % (meta["version"], meta["commit"] or "unknown",
                                              meta["date"] or "unknown"), ""]
    for key, name in BINARIES:
        if key not in SHOWN:
            continue
        o.append("## %s" % name)
        o.append("")
        o.append("| Category | Matched | Equivalent | Total | Percent |")
        o.append("|---|---:|---:|---:|---:|")
        extra = sorted(c for c in cats[key] if c not in CATEGORIES)
        for c in CATEGORIES + extra:
            d = cats[key].get(c, empty())
            fn, b = d["functions"], d["bytes"]
            o.append("| %s | %d | %d | %d | %.1f%% |" % (
                c, fn["MATCHED"], fn["EQUIVALENT"], fn["total"], pct(b["MATCHED"] + b["EQUIVALENT"], b["total"])))
        fn, b = per[key]["functions"], per[key]["bytes"]
        o.append("| **All** | **%d** | **%d** | **%d** | **%.1f%%** |" % (
            fn["MATCHED"], fn["EQUIVALENT"], fn["total"], pct(b["MATCHED"] + b["EQUIVALENT"], b["total"])))
        o.append("")
    o += ["## Not started", ""]
    for key, name in BINARIES:
        if key not in SHOWN:
            o.append("- %s: %d functions in the CSV, all TODO%s." % (
                name, per[key]["functions"]["total"], " (%s)" % NOTES[key] if key in NOTES else ""))
    for name, note in NOT_INVENTORIED:
        o.append("- %s: %s." % (name, note))
    o += ["",
          "## Counting rules", "",
          "- **MATCHED**: the objdiff report gives 100 percent for the function: the instructions are "
          "identical to the retail binary. The report does not compare the names of relocation targets "
          "(many retail symbols are still unnamed); `tools/relocaudit.py` lists matched functions whose "
          "relocations point at differently named symbols.",
          "- **EQUIVALENT**: the function compiles and its logic was verified against the assembly, "
          "but the code is not byte identical. The remaining difference is recorded in `docs/progress.csv`.",
          "- **TODO**: every other function, including stubs and functions that do not compile.",
          "- Rows of class LIB (static CRT, STL instantiations and compiler generated helpers from the "
          "toolchain) are not decompiled and are left out of every figure.",
          "- Matched, Equivalent and Total in the tables are function counts. Percent is "
          "(MATCHED + EQUIVALENT) code bytes over all code bytes of the row; each function weighs its "
          "size in the retail binary.",
          "- The SVG bars use the same byte weighting; the function counts are shown next to them.", ""]
    with open(os.path.join(DOCS, "PROGRESS.md"), "w", newline="\n") as f:
        f.write("\n".join(o))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", default=os.path.join(DOCS, "progress.csv"))
    args = ap.parse_args()
    meta = {"version": target_version(), "commit": git("rev-parse", "--short", "HEAD"),
            "date": git("log", "-1", "--format=%cs") or datetime.date.today().isoformat()}
    per, cats = collect(args.csv)
    out = write_json(per, cats, meta)
    write_svg(per, meta)
    write_md(per, cats, meta)
    for key, name in BINARIES:
        fn, b = per[key]["functions"], per[key]["bytes"]
        print("%-13s %5d/%5d functions matched, %5d equivalent, %5.1f%% of %8d bytes matched" % (
            name, fn["MATCHED"], fn["total"], fn["EQUIVALENT"], pct(b["MATCHED"], b["total"]), b["total"]))
    t = out["total"]
    print("%-13s %5d/%5d functions matched, %5d equivalent, %5.1f%% of %8d bytes matched" % (
        "total", t["functions"]["MATCHED"], t["functions"]["total"], t["functions"]["EQUIVALENT"],
        pct(t["bytes"]["MATCHED"], t["bytes"]["total"]), t["bytes"]["total"]))


if __name__ == "__main__":
    main()
