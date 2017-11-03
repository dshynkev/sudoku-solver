import array
import time
import sys


class SudokuSolver:
    """A 9 by 9 sudoku solver"""
    # Main side length
    MAJOR = 9
    # Partition size length
    MINOR = 3
    # Empty value
    EMPTY = 0

    FORMATTING_GRID = """
    -------------------------
    | {} {} {} | {} {} {} | {} {} {} |
    | {} {} {} | {} {} {} | {} {} {} |
    | {} {} {} | {} {} {} | {} {} {} |
    --------+-------+--------
    | {} {} {} | {} {} {} | {} {} {} |
    | {} {} {} | {} {} {} | {} {} {} |
    | {} {} {} | {} {} {} | {} {} {} |
    --------+-------+--------
    | {} {} {} | {} {} {} | {} {} {} |
    | {} {} {} | {} {} {} | {} {} {} |
    | {} {} {} | {} {} {} | {} {} {} |
    -------------------------
    """

    def __init__(self, file=None):
        # We know the size of the gird of advance, so we can optimize our
        # storage by using a fixed-size array
        self.grid = array.array('b')

        if file is not None:
            self.init_from_file(file)

    def __repr__(self):
        return self.FORMATTING_GRID.format(*map(lambda x: '.' if x == 0 else x, self.grid))

    def init_grid(self, strrep: [str]):
        """
        Initialize the grid from its string representation.
        The input should be in the form of `major` lines of `major` digits.
        Empty cells should be filled with 0's.
        """

        for line in strrep:
            for num in line.split():
                if num == '.':
                    num = self.EMPTY
                else:
                    num = int(num)
                if num < 0 or num > self.MAJOR:
                    raise ValueError("%d is not a decimal digit" % num)
                self.grid.append(num)

    def init_from_file(self, file):
        """Initialize the grid from a given file"""
        with open(file) as f:
            self.init_grid(f.readlines())

    def next_empty(self):
        """Return the next vacant cell in the grid"""
        for i in range(self.MAJOR):
            for j in range(self.MAJOR):
                if self.grid[i * self.MAJOR + j] == self.EMPTY:
                    return (i, j)
        return None

    def is_legal(self, guess, i, j):
        """
        Check if a given value is legal at given coordinates.
        For a value to be legal, it has to be unique in its:
            - row
            - column
            - `minor` x `minor` partition
        """

        # Checked column
        for col in range(self.MAJOR):
            if col == j:
                continue
            if self.grid[i * self.MAJOR + col] == guess:
                return False
        # Check row
        for row in range(self.MAJOR):
            if row == i:
                continue
            if self.grid[row * self.MAJOR + j] == guess:
                return False
        # Check partition
        for row in range(self.MINOR * (i // self.MINOR),
                         self.MINOR * (i // self.MINOR + 1)):
            for col in range(self.MINOR * (j // self.MINOR),
                             self.MINOR * (j // self.MINOR + 1)):
                if row == i and col == j:
                    continue
                if self.grid[row * self.MAJOR + col] == guess:
                    return False
        return True

    def solve(self, quiet=False, delay=0):
        """
        Solve the current grid.
        The grid must be filled in before this method is called.

        Arguments:
            - quiet -- set to True to disable intermittent output.
                       False by default.
            - delay -- delay when printing (implies quiet == False)
        Returns:
            - True if the puzzle was solved
            - False if it is impossible to solve it.

        The algorithm used here is very simple:
            - if there is a legal move to make, make it and recurse
            - if a legal move is not possible, backtrack
            - if backtracked out to the caller, the puzzle has no solution
        """

        cell = self.next_empty()
        # If there are no empty cells, we must have solved the puzzle
        if cell is None:
            return True

        i, j = cell
        for guess in range(1, self.MAJOR + 1):
            if not quiet:
                print(self)
                time.sleep(delay)
                # Go 16 rows up to create an illusion of static output
                print('\033[F' * 16)
            if self.is_legal(guess, i, j):
                self.grid[i * self.MAJOR + j] = guess
                if self.solve(quiet, delay):
                    return True
        # This cell could not be solved => set back to zero
        self.grid[i * self.MAJOR + j] = 0
        return False


def main():
    if len(sys.argv) != 3:
        print("Usage: {} [FILENAME] [DELAY]".format(sys.argv[0]))
        return False
    solver = SudokuSolver(sys.argv[1])
    delay = float(sys.argv[2])
    if delay< 0:
        quiet = True
    else:
        quiet = False

    start = time.clock()
    solver.solve(quiet, delay)
    end = time.clock()
    # Go 16 lines down to print the results without overwriting the output
    print(solver)
    print("It took us {} seconds of CPU time to solve the puzzle."
          .format(end - start))


if __name__ == "__main__":
    main()
