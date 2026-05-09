import curses
import asyncio
from conway import Next_Generation_Ref

class World:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(World, cls).__new__(cls)
        return cls._instance

    def __init__(self):
        if hasattr(self, 'initialized') and self.initialized:
            return
        
        # calculated grid
        self.grid = None

        # singleton flag
        self.initialized = True

        # padding between upper left corners of grid & window
        self.OFFSET = 3
        # evolving pause time 
        self.INTERVAL = 1
        # visual origin
        self.VORIGIN = (0, 0)
        # grid upper left corner coordination
        self.ul = (0, 0)
        # evolving generation
        self.generation = 0

        # stepping mode
        self.PAUSED = False
        # set True when J is pressed
        self.STEP_ONCE = False

        # when q is pressed, set stop_event to break sleep and quit immediately
        self.stop_event = None
        # when space is pressed, set pause_event to break sleep and pause immediately
        self.pause_event = None
        # exit loop flag
        self.STOP_FLAG = False

    def print_grid(self, stdscr, done=False):
        stdscr.clear()
        ul = (self.ul[0] - self.VORIGIN[0], self.ul[1] - self.VORIGIN[1])

        max_y, max_x = stdscr.getmaxyx()
        
        display_height = max_y - 3
        display_width = max_x // 2

        display_grid = [['  ' for _ in range(display_width)] for _ in range(display_height)]

        for r, row_data in enumerate(self.grid):
            for c, cell in enumerate(row_data):
                if cell:
                    display_r = r + ul[0] + self.OFFSET
                    display_c = c + ul[1] + self.OFFSET
                    if 0 <= display_r < display_height and 0 <= display_c < display_width:
                        display_grid[display_r][display_c] = '[]'
        
        for i, row in enumerate(display_grid):
            if i >= max_y - 3:
                break
            line = ''.join(row)
            if len(line) >= max_x:
                line = line[:max_x - 1]
            stdscr.addstr(i, 0, line)

        status_line = f"upper left corner: {self.ul}"
        if self.PAUSED:
            status_line += " [PAUSED]"
        
        stdscr.addstr(max_y - 3, 0, status_line)
        stdscr.addstr(max_y - 2, 0, f"VORIGIN: {self.VORIGIN}, use arrow keys to move, 'q' to quit, 'space' to pause, 'j' to step.")
        if done:
            stdscr.addstr(max_y - 1, 0, f"Stabilized at Generation {self.generation}.")
        else:
            stdscr.addstr(max_y - 1, 0, f"Interval: {self.INTERVAL:.2f}s (+/- to change)")
        stdscr.refresh()

    async def handle_input_curses(self, stdscr):
        stdscr.nodelay(1)
        while not self.STOP_FLAG:
            key = stdscr.getch()
            if key != -1:
                update = True
                if key == curses.KEY_UP:
                    self.VORIGIN = (self.VORIGIN[0] - 1, self.VORIGIN[1])
                elif key == curses.KEY_DOWN:
                    self.VORIGIN = (self.VORIGIN[0] + 1, self.VORIGIN[1])
                elif key == curses.KEY_LEFT:
                    self.VORIGIN = (self.VORIGIN[0], self.VORIGIN[1] - 1)
                elif key == curses.KEY_RIGHT:
                    self.VORIGIN = (self.VORIGIN[0], self.VORIGIN[1] + 1)
                elif key == ord('+') or key == ord('='):
                    self.INTERVAL = max(0.1, self.INTERVAL + 0.1)
                elif key == ord('-') or key == ord('_'):
                    self.INTERVAL = min(2.0, self.INTERVAL - 0.1)
                elif key == ord('q'):
                    self.STOP_FLAG = True
                    if self.stop_event:
                        self.stop_event.set()
                elif key == ord(' '):
                    self.PAUSED = not self.PAUSED
                    if self.pause_event:
                        self.pause_event.set()
                elif key == ord('j'):
                    if self.PAUSED:
                        self.STEP_ONCE = True
                else:
                    update = False
                if update:
                    self.print_grid(stdscr)
            await asyncio.sleep(0.01)

    def evolve(self):
        """Evolves the grid to the next generation."""
        if self.grid is None:
            return False

        prev_grid = [row[:] for row in self.grid]
        # replace with your own next_generation function!
        self.grid, dg = Next_Generation_Ref(self.grid)
        self.ul = (self.ul[0] + dg[0], self.ul[1] + dg[1])
        self.generation += 1
        
        return self.grid != prev_grid

    async def game_loop_curses(self, stdscr, iter_limit):
        curses.curs_set(0)
        stdscr.nodelay(1)

        self.STOP_FLAG = False
        self.generation = 0
        self.ul = (0, 0)
        self.PAUSED = False
        self.STEP_ONCE = False
        self.stop_event = asyncio.Event()
        self.pause_event = asyncio.Event()
        
        input_task = asyncio.create_task(self.handle_input_curses(stdscr))
        
        self.print_grid(stdscr)

        for _ in range(iter_limit):

            while self.PAUSED and not self.STEP_ONCE and not self.STOP_FLAG:
                await asyncio.sleep(0.01)
            
            if self.STOP_FLAG:
                break

            # 暂停后单步
            if self.STEP_ONCE:
                self.STEP_ONCE = False
                if not self.evolve():
                    stdscr.refresh()
                    
                    sleep_task = asyncio.create_task(asyncio.sleep(2))
                    await asyncio.wait(
                        [sleep_task, asyncio.create_task(self.stop_event.wait())],
                        return_when=asyncio.FIRST_COMPLETED
                    )
                    if self.stop_event.is_set():
                        sleep_task.cancel()
                    
                    break
                self.print_grid(stdscr)
                continue

            # 等待主体，可以被暂停/退出中断
            sleep_task = asyncio.create_task(asyncio.sleep(self.INTERVAL))
            await asyncio.wait(
                [sleep_task, asyncio.create_task(self.stop_event.wait()), asyncio.create_task(self.pause_event.wait())],
                return_when=asyncio.FIRST_COMPLETED
            )
            if self.stop_event.is_set():
                sleep_task.cancel()
                break
            if self.pause_event.is_set():
                self.pause_event.clear()
                sleep_task.cancel()
                self.print_grid(stdscr)
                continue

            if not self.PAUSED or self.STEP_ONCE:

                if not self.evolve():
                    stdscr.refresh()
                    
                    sleep_task = asyncio.create_task(asyncio.sleep(2))
                    await asyncio.wait(
                        [sleep_task, asyncio.create_task(self.stop_event.wait())],
                        return_when=asyncio.FIRST_COMPLETED
                    )
                    if self.stop_event.is_set():
                        sleep_task.cancel()
                    
                    break
                self.print_grid(stdscr)
                
                if self.STEP_ONCE:
                    self.STEP_ONCE = False
        
        input_task.cancel()

world = World()

def Expand_Visualize(grid, iter_limit):
    world.grid = grid

    def run_async_loop(stdscr):
        return asyncio.run(world.game_loop_curses(stdscr, iter_limit))
    
    return curses.wrapper(run_async_loop)