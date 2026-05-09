import os, time, random, argparse

from conway import *

from visualize import Expand_Visualize

def initialize_grid(height, width):
    return [[random.choice([0, 1]) for _ in range(width)] for _ in range(height)]

def read_rte_file(file_path):
    with open(file_path, 'r') as f:
        lines = f.readlines()

    width, height = 0, 0
    rle_string = ""
    header_found = False

    for line in lines:
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        if not header_found and ('x' in line and 'y' in line):
            parts = line.split(',')
            for part in parts:
                part = part.strip()
                if part.startswith('x'):
                    width = int(part.split('=')[1])
                elif part.startswith('y'):
                    height = int(part.split('=')[1])
            header_found = True
        else:
            rle_string += line

    if '!' in rle_string:
        rle_string = rle_string[:rle_string.find('!')]

    if width == 0 or height == 0:
        raise ValueError("Could not find grid dimensions in the RLE file.")

    grid = [[0 for _ in range(width)] for _ in range(height)]
    x, y = 0, 0
    run_count = 0

    for char in rle_string:
        if char.isdigit():
            run_count = run_count * 10 + int(char)
        else:
            count = max(1, run_count)
            if char == 'o':
                for _ in range(count):
                    if x < width:
                        grid[y][x] = 1
                        x += 1
            elif char == 'b':
                x += count
            elif char == '$':
                y += count
                x = 0
            run_count = 0
            
    return grid

def trim_grid(grid):
    if not grid or not any(any(row) for row in grid):
        return [[0]]

    min_r, max_r = -1, -1
    min_c, max_c = len(grid[0]), -1

    for r, row in enumerate(grid):
        if any(row):
            if min_r == -1:
                min_r = r
            max_r = r
            for c, cell in enumerate(row):
                if cell == 1:
                    min_c = min(min_c, c)
                    max_c = max(max_c, c)
    
    if min_r == -1:
        return [[0]]

    return [row[min_c:max_c+1] for row in grid[min_r:max_r+1]]

def main():
    parser = argparse.ArgumentParser(description="Conway's Game of Life simulation.")
    parser.add_argument('-V', '--visualize', action='store_true', help='Enable visualization.')
    parser.add_argument('-I', '--iter', type=int, default=100, help='Number of iterations.')
    
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-F', '--file', type=str, help='Path to an RTE file to load the initial grid.')
    group.add_argument('-S', '--size', type=int, nargs=2, metavar=('HEIGHT', 'WIDTH'), help='Height and width for a random grid.')

    args = parser.parse_args()

    grid = []
    if args.file:
        grid = read_rte_file(args.file)
    elif args.size:
        height, width = args.size
        grid = initialize_grid(height, width)
    
    if args.visualize:
        os.system("cls" if os.name == 'nt' else 'clear')
        Expand_Visualize(grid, args.iter)
        return

    grid_ref = grid
    start_Ref = time.perf_counter()
    ans_Ref = Expand_Ref(grid_ref, args.iter)
    end_Ref = time.perf_counter()
    
    start = time.perf_counter()
    ans = Expand(grid, args.iter)
    end = time.perf_counter()

    trimmed_ans = trim_grid(ans)
    trimmed_ans_Ref = trim_grid(ans_Ref)

    if trimmed_ans != trimmed_ans_Ref:
        print(f" final result different")
        print("Trimmed Optimized:")
        for row in trimmed_ans:
            print(row)
        print("Trimmed Reference:")
        for row in trimmed_ans_Ref:
            print(row)
        print("WA")
        exit()
    
    print("AC")
    print(f"Reference: {end_Ref - start_Ref:.4f} s")
    print(f"Optimized: {end - start:.4f} s")


if __name__ == "__main__":
    main()
