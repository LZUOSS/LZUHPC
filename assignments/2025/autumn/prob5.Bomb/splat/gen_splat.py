#!/usr/bin/env python3
#
# gen_splat.py: Configuration generator for the event splatting problem.
#
# Usage:
#   python3 gen_splat.py --size {S,M,L} --seed <integer>
#
# Output format (space-separated values to stdout):
#   H W bombs_count bombs_r_min bombs_r_max bombs_gaussian_flag
#   bullets_count bullets_r_min bullets_r_max events_seed
#

import sys
import argparse
import random

def main():
    sizes = {
    'S': (2048 * 2, 2048 * 2, 100000, 500000),
    'M': (2048 * 4, 2048 * 4, 300000, 600000),
    'L': (2048 * 8, 2048 * 8, 500000, 800000)
    }


    parser = argparse.ArgumentParser(
        description="Generate configuration for the event splatting optimization problem."
    )
    parser.add_argument(
        '--size',
        choices=sizes.keys(),
        required=True,
        help="Problem size (S, M, L)."
    )
    parser.add_argument(
        '--seed',
        type=int,
        default=42,
        help="Seed for the random number generator."
    )
    args = parser.parse_args()
    rnd = random.Random(args.seed)
    H, W, num_bombs, num_bullets = sizes[args.size]

    bombs_r_min = 10
    bombs_r_max = 40
    bombs_gaussian = rnd.choice([True, False])
    bullets_r_min = 1
    bullets_r_max = 4

    events_seed = rnd.randrange(1, 2**32)

    output_values = [
        H, W,
        num_bombs,
        bombs_r_min,
        bombs_r_max,
        1 if bombs_gaussian else 0, 
        num_bullets,
        bullets_r_min,
        bullets_r_max,
        events_seed
    ]

    print(' '.join(map(str, output_values)))

if __name__ == '__main__':
    main()