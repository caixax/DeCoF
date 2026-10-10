"""Write objdiff results back into docs/progress.csv.

Usage: python tools/update_status.py [--report build/report.json]

For every function in the objdiff report: 100% -> status MATCHED and
match_percent 100; below 100 -> match_percent updated, status left as is
(TODO or EQUIVALENT are decided by hand). Functions are located by their
decorated name through tools/symbols/<dll>_1.6a.csv. Unit names in the report
are "<dll>/<path>".
"""
import argparse
import csv
import json
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--report", default=os.path.join(ROOT, "build", "report.json"))
    args = ap.parse_args()
    rep = json.load(open(args.report))
    path = os.path.join(ROOT, "docs", "progress.csv")
    rows = list(csv.DictReader(open(path)))
    fields = list(rows[0].keys())
    index = {(r["dll"], r["va"]): r for r in rows}
    symbols = {}
    changed = 0
    for unit in rep.get("units", []):
        dll = unit["name"].split("/")[0]
        if dll not in symbols:
            sp = os.path.join(ROOT, "tools", "symbols", "%s_1.6a.csv" % dll)
            symbols[dll] = {r["name"]: r["va"] for r in csv.DictReader(open(sp))} if os.path.exists(sp) else {}
        for fn in unit.get("functions", []):
            va = symbols[dll].get(fn["name"])
            row = index.get((dll, va)) if va else None
            if row is None:
                continue
            pct = fn.get("fuzzy_match_percent", 0.0)
            new_status = "MATCHED" if pct >= 100.0 else row["status"]
            new_pct = "100" if pct >= 100.0 else "%.1f" % pct
            if row["status"] != new_status or row["match_percent"] != new_pct:
                row["status"], row["match_percent"] = new_status, new_pct
                changed += 1
    with open(path, "w", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=fields, lineterminator="\n")
        w.writeheader()
        w.writerows(rows)
    print("updated %d rows" % changed)


if __name__ == "__main__":
    main()
