# Millisecond Sudoku Solver

## Usage

To run the program, download and compile `solver.c`. Update `problem.txt` to the Sudoku problem you wish to solve using the format of the example below. When run, the program will print all possible solutions to the problem, or output that no solution exists if that's the case. It will also print how long it took to solve. **Easy problems will be solved in under 1 ms, and even the hardest problems shouldn't take more than a couple milliseconds.**

Example `problem.txt`:
```
8........
..36.....
.7..9.2..
.5...7...
....457..
...1...3.
..1....68
..85...1.
.9....4..
```
Note: According to [The Telegraph](https://www.telegraph.co.uk/news/science/science-news/9359579/Worlds-hardest-sudoku-can-you-crack-it.html), the above is supposed to be the world's hardest Sudoku.

## How it works

### Abstracting the Sudoku problem

The way we solve this is by translating a Sudoku puzzle to a binary 2D matrix (which we will consider to be true/false values). The rows represent choices, and the columns represent constraints.

The binary matrix looks something like [this](https://www.stolaf.edu/people/hansonr/sudoku/exactcovermatrix.htm).

For example, a row may represent placing a 5 in position (1,3). This row would have exactly four 'true' entries:
1. One in the column that represents having a 5 in row 3.
2. One in the column that represents having a 5 in column 1.
3. One in the column that represents having a 5 in the top-left square.
4. One in the column that represents having a number in position (1,3).

All other entries in the row would be false. For example, the entry in the column that represents having a 9 in row 3 would be false since that isn't satisfied by this row's choice.

A filling of a Sudoku grid can be represented by its corresponding rows (placed numbers). A valid filling of a Sudoku grid must consist of enough rows such that every column has exactly one true entry. This is called an exact cover problem.
The numbers we are given to start a Sudoku problem represent rows already chosen for us. We must then pick enough remaining rows to fill each column exactly once.

### Solving the exact cover problem

**Key concept**: Consider what happens when we choose a row to add to our solution. It will fill four columns. But we don't want those columns to be filled a second time by some other row we pick, since that would make it an invalid filling. So for each column satisfied by our chosen row, we want to remove all rows which also fill that column since they are incompatible with our solution. In fact, we can just remove the columns too, since we don't need to worry about them anymore. When we do this to a column, we say we *cover* it.

Now, we start the problem with some rows already chosen for us (prefilled numbers). We cover the columns filled by these rows. Now we want to solve the resulting exact cover problem recursively as follows:
1. Check if there are no columns left (they have all been covered). If so, we have found a solution! We can print it and return.
2. If there are columns left, we pick one.
3. We still need to fill this column, so we iterate through the remaining rows which fill it (that have not been removed by covering).
4. For each row, we try adding it to our solution, covering the columns it fills, recursively solving from there, and then undo this to try the next row.
5. Once we've tried all the rows in our chosen column, we can return, since any solution from this point would need to fill our chosen column and therefore would include one of those rows we tried.

### Speeding things up

When we pick our column in step 2 of the recursion, we can be a bit smarter and pick the most efficient column. Since we need to iterate through the rows which fill it, this is the column with the fewest rows that fill it. Thinking of recursion as a tree, this keeps it as skinny as possible near the root, which speeds up our process.

You might notice we do a lot of removing and adding back of rows and columns. These operations can be very expensive and greatly slow down our program due to how often they occur during our recursion. To cut down on this cost, we can represent our binary 2D matrix using a 2D "grid" of doubly-linked lists. Each node is an entry in the matrix, with a link to the node above, below, to the left, and to the right. To remove a node from the matrix, we don't remove it from memory, but just update its neighbours to skip over it. But the node itself still knows where it is and where its neighbours are, so it can add itself back to the matrix by updating its neighbours again. We can add headers to this structure to facilitate scanning through our columns and accessing our matrix. This reduces all changes to the matrix to constant time operations.

### Credits

The dancing links algorithm was created by Donald Knuth. You can can read his excellent paper on it [here](https://arxiv.org/pdf/cs/0011047.pdf).

(Code moved from a previous private repository.)
