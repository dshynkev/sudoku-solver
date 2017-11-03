#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAJOR 9
#define MINOR 3
#define EMPTY 0
#define NONE -1
#define BUFFER_SIZE 256

typedef int8_t sudoku_grid_t[9][9];

/*
** Find the next empty cell in the grid.
*/
static void next_empty(sudoku_grid_t sudoku, int *row, int *col)
{
	for (int i = 0; i < MAJOR; ++i) {
		for (int j = 0; j < MAJOR; ++j) {
			if (sudoku[i][j] == EMPTY) {
				*row = i;
				*col = j;
				return;
			}
		}
	}
	*row = NONE;
	*col = NONE;
}

/*
** Check if a given value is legal at given coordinates.
** For a value to be legal, it has to be unique in its:
**		- row
**		- column
**		- MINOR x MINOR partition
*/
static bool is_legal(sudoku_grid_t sudoku, int8_t guess, int row, int col)
{
	// Check row
	for (int i = 0; i < MAJOR; ++i) {
		if (row == i)
			continue;
		if (sudoku[i][col] == guess)
			return false;
	}

	// Check column
	for (int j = 0; j < MAJOR; ++j) {
		if (col == j)
			continue;
		if (sudoku[row][j] == guess)
			return false;
	}

	// Check partition
	for (int i = MINOR*(row/MINOR); i < MINOR*(row/MINOR + 1); ++i) {
		for (int j = MINOR*(col/MINOR); j < MINOR*(col/MINOR + 1); ++j) {
			if (row == i && col == j)
				continue;
			if (sudoku[i][j] == guess)
				return false;
		}
	}

	return true;
}

/*
** Solve the puzzle. The grid must be filled in before this function is called.
**
** Returns:
**	- True if the puzzle was solved
**	- False if it is impossible to solve it.
**
** The algorithm used here is very simple:
**	- if there is a legal move to make, make it and recurse
**	- if a legal move is not possible, backtrack
**	- if backtracked out to the caller, the puzzle has no solution
*/
static bool solve_sudoku(sudoku_grid_t sudoku)
{
	int row, col;
	next_empty(sudoku, &row, &col);
	// If there are no empty cells, we solved the puzzle
	if (row == -1 && col == -1)
		return true;

	for (int8_t guess = 1; guess <= MAJOR; ++guess) {
		if (is_legal(sudoku, guess, row, col)) {
			sudoku[row][col] = guess;
			if (solve_sudoku(sudoku))
				return true;
		}
	}
	// This cell could not be solved => zero it out
	sudoku[row][col] = EMPTY;
	return false;
}

/*
** Read sudoku from a given file.
**
** Empty cells must be represented as EMPTY or the character '.'
*/
static bool read_sudoku(sudoku_grid_t sudoku, FILE *file)
{
	char buffer[BUFFER_SIZE];

	// Read one line at a time
	for (int i = 0; i < MAJOR; ++i) {
		int j = 0;
		// Plant a canary
		buffer[0] = '\0';
		// Read from file (the cast is needed to silence a warning)
		fgets((char *__restrict)&buffer, BUFFER_SIZE, file);
		// If the canary is still there, nothing was read from the file.
		if (buffer[0] == '\0') {
			fprintf(stderr, "Not enough lines of input: line %d missing\n", i + 1);
			return false;
		}


		// Parse the line
		for (int pos = 0; buffer[pos] != '\0'; ++pos) {
			if (buffer[pos] >= '0' && buffer[pos] <= '9') {
				sudoku[i][j] = buffer[pos] - '0'; 
				++j;
			// Also interpret dots as empty cells
			} else if (buffer[pos] == '.') {
				sudoku[i][j] = EMPTY;
				++j;
			}

			// If there are too many values on the same linem
			// bail out with an error (detected outside of the for-loop)
			if (j > MAJOR) {
				j = 0;
				break;
			}
		}

		// If there were not enough values (or too many), the input is invalid
		if (j < MAJOR) {
			fprintf(stderr, "Faulty input on line %d\n", i + 1);
			return false;
		}
	}

	return true;
}

/*
** Print the sudoku grid out as text.
*/
static void print_sudoku(sudoku_grid_t sudoku)
{
	// Upper border
	printf(" -------------------------\n");
	for (int i = 0; i < MAJOR; ++i) {
		// Horizontal partition separator
		if ((i != 0) && (i % MINOR == 0))
			printf(" --------+-------+--------\n");
		for (int j = 0; j < MAJOR; ++j) {
			// Left border and vertical partition separator
			if (j % MINOR == 0)
				printf(" |");
			if (sudoku[i][j] == EMPTY)
				// Empty cells are printed as dots
				printf(" .");
			else
				// Everything else - as numbers
				printf(" %d", sudoku[i][j]);
		}
		// Right border
		printf(" |\n");
	}
	// Lower border
	printf(" -------------------------\n");
}

int main(int argc, const char *argv[])
{
	sudoku_grid_t sudoku;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s FILENAME\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	FILE *file = fopen(argv[1], "rb");

	if (!read_sudoku(sudoku, file))
		exit(EXIT_FAILURE);

	solve_sudoku(sudoku);
	print_sudoku(sudoku);

	exit(EXIT_SUCCESS);
}