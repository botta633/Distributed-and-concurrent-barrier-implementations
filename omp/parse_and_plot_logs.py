#!/usr/bin/env python3
import argparse
import csv
import re
from pathlib import Path
from collections import defaultdict

import matplotlib.pyplot as plt


# ---------- Regexes for the lines we care about ----------
RE_THREADS_ROUNDS = re.compile(r"Threads:\s*(\d+)\s*,\s*Rounds:\s*(\d+)", re.I)
RE_WALL = re.compile(
    r"Total\s+Wall\s+Time:\s*([0-9]*\.?[0-9]+)\s*s\s*,\s*Avg/round:\s*([0-9]*\.?[0-9]+)\s*us",
    re.I,
)
RE_CPU = re.compile(
    r"Total\s+CPU\s*Time\s*:\s*([0-9]*\.?[0-9]+)\s*s\s*,\s*Avg/round:\s*([0-9]*\.?[0-9]+)\s*us",
    re.I,
)
RE_ALGO_FROM_JOBNAME = re.compile(r"Job\s+name:\s*\.\/(mp1|mp2)", re.I)


def infer_algo_from_filename(path: Path):
    """Try to infer mp1/mp2 from file name."""
    lower = path.name.lower()
    if lower.startswith("mp1"):
        return "mp1"
    if lower.startswith("mp2"):
        return "mp2"
    return None


def parse_one_file(path: Path):
    """
    Parse one .out file; return dict or None if parsing fails.
    Expected lines include:
      Threads: X, Rounds: Y
      Total Wall Time: ... s, Avg/round: ... us
      Total CPU Time : ... s, Avg/round: ... us  (optional)
    """
    content = path.read_text(errors="ignore")
    algo = infer_algo_from_filename(path)

    # Try also from "Job name: ./mp1 – 2t" if not found
    if algo is None:
        m = RE_ALGO_FROM_JOBNAME.search(content)
        if m:
            algo = m.group(1).lower()

    m = RE_THREADS_ROUNDS.search(content)
    if not m:
        return None
    threads, rounds = int(m.group(1)), int(m.group(2))

    mw = RE_WALL.search(content)
    if not mw:
        return None
    wall_total_s, wall_avg_us = float(mw.group(1)), float(mw.group(2))

    mc = RE_CPU.search(content)
    cpu_total_s = cpu_avg_us = None
    if mc:
        cpu_total_s, cpu_avg_us = float(mc.group(1)), float(mc.group(2))

    return {
        "algo": algo or "unknown",
        "threads": threads,
        "rounds": rounds,
        "wall_total_s": wall_total_s,
        "wall_avg_us": wall_avg_us,
        "cpu_total_s": cpu_total_s,
        "cpu_avg_us": cpu_avg_us,
        "file": str(path),
    }


def write_csv(rows, out_csv: Path):
    cols = [
        "algo",
        "threads",
        "rounds",
        "wall_total_s",
        "wall_avg_us",
        "cpu_total_s",
        "cpu_avg_us",
        "file",
    ]
    with out_csv.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=cols)
        w.writeheader()
        for r in rows:
            w.writerow(r)


def plot_from_rows(rows, out_png_wall: Path, out_png_cpu: Path | None = None):
    """
    Plot avg wall us (and optionally cpu us) vs threads comparing mp1 vs mp2.
    If there are multiple files per (algo, threads), it averages them.
    """
    # Aggregate avg wall us by algo, threads
    agg_wall = defaultdict(list)
    agg_cpu = defaultdict(list)

    for r in rows:
        key = (r["algo"], r["threads"])
        if r["wall_avg_us"] is not None:
            agg_wall[key].append(r["wall_avg_us"])
        if r["cpu_avg_us"] is not None:
            agg_cpu[key].append(r["cpu_avg_us"])

    def average_map(d):
        out = defaultdict(float)
        for k, vals in d.items():
            if vals:
                out[k] = sum(vals) / len(vals)
        return out

    avg_wall = average_map(agg_wall)
    avg_cpu = average_map(agg_cpu)

    def prepare_series(avg_map):
        # returns {algo: (sorted_threads_list, y_vals)}
        per_algo = defaultdict(list)
        for (algo, th), val in avg_map.items():
            per_algo[algo].append((th, val))
        series = {}
        for algo, pairs in per_algo.items():
            pairs.sort(key=lambda x: x[0])
            series[algo] = ([p[0] for p in pairs], [p[1] for p in pairs])
        return series

    # ---- Wall-time plot ----
    wall_series = prepare_series(avg_wall)
    plt.figure(figsize=(7, 4))
    for algo, (x, y) in wall_series.items():
        plt.plot(x, y, marker="o", label=algo)
    plt.xlabel("Threads")
    plt.ylabel("Avg wall time per round (us)")
    plt.title("Barrier Avg Wall Time vs Threads")
    plt.grid(True, linestyle="--", alpha=0.4)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_png_wall)
    plt.close()

    # ---- CPU-time plot ----
    if avg_cpu and out_png_cpu is not None:
        cpu_series = prepare_series(avg_cpu)
        plt.figure(figsize=(7, 4))
        for algo, (x, y) in cpu_series.items():
            plt.plot(x, y, marker="o", label=algo)
        plt.xlabel("Threads")
        plt.ylabel("Avg CPU time per round (us)")
        plt.title("Barrier Avg CPU Time vs Threads")
        plt.grid(True, linestyle="--", alpha=0.4)
        plt.legend()
        plt.tight_layout()
        plt.savefig(out_png_cpu)
        plt.close()


def main():
    ap = argparse.ArgumentParser(
        description="Parse mp1/mp2 timing logs, write CSV & generate comparison plots."
    )
    ap.add_argument(
        "log_dir", type=Path, help="Directory containing *.out log files (e.g., ./logs)"
    )
    ap.add_argument(
        "--csv",
        type=Path,
        default=Path("results.csv"),
        help="Output CSV path (default: results.csv)",
    )
    ap.add_argument(
        "--png-wall",
        type=Path,
        default=Path("avg_wall_vs_threads.png"),
        help="Output PNG for wall-time (default: avg_wall_vs_threads.png)",
    )
    ap.add_argument(
        "--png-cpu",
        type=Path,
        default=Path("avg_cpu_vs_threads.png"),
        help="Output PNG for CPU-time (default: avg_cpu_vs_threads.png)",
    )
    args = ap.parse_args()

    rows = []
    for p in sorted(args.log_dir.glob("*.out")):
        r = parse_one_file(p)
        if r is None:
            print(f"[skip] Could not parse: {p.name}")
            continue
        rows.append(r)

    if not rows:
        print("No parsable .out files found.")
        return

    write_csv(rows, args.csv)
    print(f"Wrote CSV: {args.csv}")

    # If no CPU stats present at all, we’ll skip the CPU plot.
    any_cpu = any(r["cpu_avg_us"] is not None for r in rows)
    plot_from_rows(rows, args.png_wall, args.png_cpu if any_cpu else None)
    print(f"Wrote plot: {args.png_wall}")
    if any_cpu:
        print(f"Wrote plot: {args.png_cpu}")


if __name__ == "__main__":
    main()