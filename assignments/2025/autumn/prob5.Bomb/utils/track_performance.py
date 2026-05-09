#!/usr/bin/env python3
"""
Performance Tracking Tool - Record and compare optimization progress
Usage: python3 utils/track_performance.py [save|show|compare]
"""

import json
import os
import sys
import subprocess
from datetime import datetime

PERF_LOG = ".performance_log.json"

def load_log():
    if os.path.exists(PERF_LOG):
        with open(PERF_LOG, 'r') as f:
            return json.load(f)
    return {"records": []}

def save_log(data):
    with open(PERF_LOG, 'w') as f:
        json.dump(data, f, indent=2)

def run_benchmark(size="S"):
    """Run benchmark and extract results"""
    print(f"Running benchmark (SIZE={size})...")
    
    result = subprocess.run(
        ["make", "benchmark", f"SIZE={size}"],
        capture_output=True,
        text=True
    )
    
    # Parse output
    perlin_speedup = None
    splat_speedup = None
    
    for line in result.stdout.split('\n'):
        if 'Speedup:' in line:
            # Extract speedup number
            parts = line.split('Speedup:')[1].split('x')[0].strip()
            speedup = float(parts)
            if perlin_speedup is None:
                perlin_speedup = speedup
            else:
                splat_speedup = speedup
    
    return perlin_speedup, splat_speedup

def save_performance(size="S", note=""):
    """Save current performance data"""
    perlin_speedup, splat_speedup = run_benchmark(size)
    
    if perlin_speedup is None or splat_speedup is None:
        print("❌ Unable to get performance data")
        return
    
    log = load_log()
    
    record = {
        "timestamp": datetime.now().isoformat(),
        "size": size,
        "perlin_speedup": perlin_speedup,
        "splat_speedup": splat_speedup,
        "note": note
    }
    
    log["records"].append(record)
    save_log(log)
    
    print(f"\n✓ Performance data saved (record #{len(log['records'])})")
    print(f"  Perlin: {perlin_speedup:.2f}x")
    print(f"  Splat:  {splat_speedup:.2f}x")
    if note:
        print(f"  Note: {note}")

def show_history():
    """Display history"""
    log = load_log()
    records = log["records"]
    
    if not records:
        print("No performance records yet")
        return
    
    print(f"\nPerformance Optimization History ({len(records)} records)\n")
    print("=" * 80)
    
    for i, record in enumerate(records, 1):
        timestamp = datetime.fromisoformat(record["timestamp"])
        print(f"\n#{i} - {timestamp.strftime('%Y-%m-%d %H:%M:%S')} [SIZE={record['size']}]")
        print(f"  Perlin: {record['perlin_speedup']:.2f}x")
        print(f"  Splat:  {record['splat_speedup']:.2f}x")
        if record.get("note"):
            print(f"  Note: {record['note']}")

def compare_progress():
    """Compare first and latest performance"""
    log = load_log()
    records = log["records"]
    
    if len(records) < 2:
        print("Need at least 2 records to compare")
        return
    
    first = records[0]
    latest = records[-1]
    
    print("\nPerformance Comparison\n")
    print("=" * 80)
    
    print(f"\nInitial ({datetime.fromisoformat(first['timestamp']).strftime('%Y-%m-%d %H:%M')})")
    print(f"  Perlin: {first['perlin_speedup']:.2f}x")
    print(f"  Splat:  {first['splat_speedup']:.2f}x")
    
    print(f"\nLatest ({datetime.fromisoformat(latest['timestamp']).strftime('%Y-%m-%d %H:%M')})")
    print(f"  Perlin: {latest['perlin_speedup']:.2f}x")
    print(f"  Splat:  {latest['splat_speedup']:.2f}x")
    
    print("\nProgress:")
    perlin_improve = latest['perlin_speedup'] - first['perlin_speedup']
    splat_improve = latest['splat_speedup'] - first['splat_speedup']
    
    def format_change(value):
        if value > 0:
            return f"\033[32m+{value:.2f}x ↑\033[0m"
        elif value < 0:
            return f"\033[31m{value:.2f}x ↓\033[0m"
        else:
            return "0.00x →"
    
    print(f"  Perlin: {format_change(perlin_improve)}")
    print(f"  Splat:  {format_change(splat_improve)}")
    
    total_records = len(records)
    print(f"\nTotal optimizations: {total_records}")

def main():
    if len(sys.argv) < 2:
        print("Usage:")
        print("  python3 utils/track_performance.py save [SIZE] [note]  - Save current performance")
        print("  python3 utils/track_performance.py show                - Show history")
        print("  python3 utils/track_performance.py compare             - Compare progress")
        print("\nExamples:")
        print("  python3 utils/track_performance.py save S 'optimized loops'")
        print("  python3 utils/track_performance.py show")
        print("  python3 utils/track_performance.py compare")
        return
    
    command = sys.argv[1]
    
    if command == "save":
        size = sys.argv[2] if len(sys.argv) > 2 else "S"
        note = sys.argv[3] if len(sys.argv) > 3 else ""
        save_performance(size, note)
    elif command == "show":
        show_history()
    elif command == "compare":
        compare_progress()
    else:
        print(f"Unknown command: {command}")

if __name__ == "__main__":
    main()
