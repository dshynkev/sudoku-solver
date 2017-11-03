# Sudoku solver

Invoke the Python implementation like so:

python sudoku.py INPUT_FILENAME DELAY

where the value of DELAY:
    - if non-negative, sets the time that each step remains on the screen
    - if -1, switches off verbose output and just prints the result
    - it is an error to supply any other value

-----------------------------------------

You will need to build the C implementation.
If you have a C compiler installed, just run `make sudoku`.
Invoke the C implementation like so:

./sudoku INPUT_FILENAME
