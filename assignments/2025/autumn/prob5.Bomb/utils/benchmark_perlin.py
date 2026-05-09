#!/usr/bin/env python3
import subprocess
import sys

def run_benchmark(gen_cmd, baseline_bin, submit_bin):
    def run(binary, label):
        gen = subprocess.Popen(gen_cmd, stdout=subprocess.PIPE, text=True)
        try:
            proc = subprocess.run(
                ["./" + binary],
                stdin=gen.stdout,
                capture_output=True,
                text=True,
                check=True,
            )
        finally:
            gen.stdout.close()
            ret = gen.wait()
            if ret != 0:
                print(f"[error] generator exited with code {ret}", file=sys.stderr)
                sys.exit(ret)

        elapsed = None
        for line in proc.stderr.splitlines():
            if line.startswith("time_sec="):
                elapsed = float(line.split("=", 1)[1])
                break
        if elapsed is None:
            print(f"[error] Unable to parse timing for {label}", file=sys.stderr)
            sys.exit(1)
        print(f"  {label:<10} hash={proc.stdout.strip()} time_sec={elapsed:.6f}")
        return elapsed

    baseline_time = run(baseline_bin, "baseline")
    submit_time = run(submit_bin, "submission")

    if submit_time == 0:
        speedup = float("inf")
    else:
        speedup = baseline_time / submit_time
    
    # Colored output with performance rating
    print()
    print("  " + "="*60)
    if speedup >= 2.0:
        color = "\033[1;32m"  # Bright green - Excellent
        rating = "🌟 Excellent"
    elif speedup >= 1.5:
        color = "\033[32m"    # Green - Good
        rating = "✓ Good"
    elif speedup >= 1.0:
        color = "\033[33m"    # Yellow - Pass
        rating = "○ Pass"
    else:
        color = "\033[31m"    # Red - Needs work
        rating = "✗ Needs work"
    
    print(f"  {color}Speedup: {speedup:.2f}x  [{rating}]\033[0m")
    
    if speedup > 1.0:
        improvement = (speedup - 1.0) * 100
        print(f"  Performance gain: {improvement:.1f}%")
    elif speedup < 1.0:
        regression = (1.0 - speedup) * 100
        print(f"  Performance loss: {regression:.1f}%")
    
    print("  " + "="*60)

if __name__ == "__main__":
    if len(sys.argv) < 5:
        print("Usage: benchmark.py <size> <seed> <baseline_bin> <submit_bin>", file=sys.stderr)
        sys.exit(1)
    
    size = sys.argv[1]
    seed = sys.argv[2]
    baseline_bin = sys.argv[3]
    submit_bin = sys.argv[4]
    
    gen_cmd = ["python3", "gen_perlin.py", "--size", size, "--seed", seed]
    run_benchmark(gen_cmd, baseline_bin, submit_bin)
