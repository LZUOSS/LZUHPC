# return: grid, (dy, dx)
# (dy, dx) indicates the movement of upper left corner, only used for visualization
def Next_Generation_Ref(grid):
    if not grid or not grid[0]:
        return [], (0, 0)

    height, width = len(grid), len(grid[0])
    padded_grid = [[0] * (width + 2) for _ in range(height + 2)]
    for y in range(height):
        for x in range(width):
            padded_grid[y + 1][x + 1] = grid[y][x]

    next_padded_grid = [[0] * (width + 2) for _ in range(height + 2)]
    
    for y in range(height + 2):
        for x in range(width + 2):
            live_neighbors = 0
            for i in range(-1, 2):
                for j in range(-1, 2):
                    if i == 0 and j == 0:
                        continue
                    if y + i >= 0 and y + i < height + 2 and x + j >= 0 and x + j < width + 2:
                        live_neighbors += padded_grid[y + i][x + j]

            next_padded_grid[y][x] = live_neighbors == 3 or (live_neighbors == 2 and padded_grid[y][x] == 1)

    min_y, max_y = -1, -1
    min_x, max_x = -1, -1

    for y in range(height + 2):
        for x in range(width + 2):
            if next_padded_grid[y][x] == 1:
                if min_y == -1 or y < min_y: 
                    min_y = y
                if max_y == -1 or y > max_y: 
                    max_y = y
                if min_x == -1 or x < min_x: 
                    min_x = x
                if max_x == -1 or x > max_x: 
                    max_x = x
    
    if min_y == -1:
        return [], (0, 0)

    new_grid = []
    for y in range(min_y, max_y + 1):
        new_grid.append(list(next_padded_grid[y][min_x : max_x + 1]))

    return new_grid, (min_y - 1, min_x - 1)

def Expand_Ref(grid, iter):
    generation = 0
    while True : 
        prev_grid = [row[:] for row in grid]
        generation = generation + 1
        grid, _ = Next_Generation_Ref(grid)
        if generation >= iter or grid == prev_grid:
            break
    return grid

#这里是你可以修改的区域
import NG
def Expand(grid, iter):
    # Implement your own version to calculate the final grid
    # return NG.Expand_Cpp(grid, iter)
    return Expand_Ref(grid, iter)