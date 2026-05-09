import sys
import argparse
import random

def main():
    sizes = {
        'S': (2048 * 2, 2048 * 2),
        'M': (2048 * 4, 2048 * 4),
        'L': (2048 * 8, 2048 * 8)
    }

    parser = argparse.ArgumentParser(
        description="Generate configuration for the Perlin noise optimization problem."
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
    H, W = sizes[args.size]

    octaves = rnd.randrange(3, 6) 
    persistence = 0.5
    lacunarity = 2.0
    base_freq = 1.0 / 64.0
    height_scale = 40
    perlin_seed = rnd.randrange(1, 2**32) 

    output_values = [
        H, W,
        octaves,
        persistence,
        lacunarity,
        base_freq,
        height_scale,
        perlin_seed
    ]

    print(' '.join(map(str, output_values)))

if __name__ == '__main__':
    main()