/*
SUDOKU SOLVER USING DANCING LINKS
Essentially, the way this solver works is it translates a sudoku puzzle to a binary 2D matrix. The rows represent options (ex. a 9 at row 3 and col 7), and the cols represent constraints (ex. there needs to be a 2 in row 5). This matrix, once you remove the  rows representing the numbers given by the problem (and their columns) is an exact cover problem, meaning you have to find a combination of rows that satisfies each column exactly once. The most efficient way to do this is to turn our 2D matrix into a doubly linked list. We can solve this list using an algorithm called Dancing Links developed by Donald Knuth. A link to Knuth's paper can be found below, and I recommend reading it.

Steps:
1. Create a 2D binary matrix (with the patterns shown in the link below) with pointers to a linked matrix.
2. Create headers for the linked matrix.
3. Assign pointers to linked matrix using 2D matrix.
4. Load & cover numbers given by sudoku problem.
5. Recursively solve remaing exact matrix.
a) check if no headers remain, if so, print solution,
b) find S heuristic (column with the least rows), as it greatly speeds up the solving proccess,
c) iterate through the rows in that column, and try covering them one by one and continuing with that row,
d) after all possibilities with that row are explored, undo that row, and try the next,

Dancing links paper: https://www.ocf.berkeley.edu/~jchu/publicportal/sudoku/0011047.pdf
Sudoku exact cover binary matrix: https://www.stolaf.edu/people/hansonr/sudoku/exactcovermatrix.htm
*/


#include <stdio.h>
#include <stdbool.h>

// linked matrix node
struct node {
    struct node *up;
    struct node *down;
    struct node *left;
    struct node *right;
    struct node *header;

    int num;
    int row;
    int col;
};

// 2d matrix node
struct node2d {
    bool filled;
    struct node *pointer;
};

// cover function
void cover(struct node *col) {
    col->left->right = col->right;
    col->right->left = col->left;

    for (struct node *n1 = col->down; n1 != col; n1 = n1->down) {
        for (struct node *n2 = n1->right; n2 != n1; n2 = n2->right) {
            n2->up->down = n2->down;
            n2->down->up = n2->up;
        }
    }
}

// uncover function
void uncover(struct node *col) {
    col->left->right = col;
    col->right->left = col;

    for (struct node *n1 = col->up; n1 != col; n1 = n1->up) {
        for (struct node *n2 = n1->left; n2 != n1; n2 = n2->left) {
            n2->up->down = n2;
            n2->down->up = n2;
        }
    }
}

// linked matrix & sudoku grid & number of solutions (global for solve)
struct node matrix[10*9*9*4+1];
int sudoku[9][9];
int solutions = 0;

// recursive solve function
void solve() {
    // if no columns remain uncovered print solution
    if (matrix[0].right == &matrix[0]) {
        solutions++;
        printf("SOLUTION %d\n", solutions);
        for (int i = 0; i < 9; i++) {
            if (i == 3 || i == 6) printf("------+-------+------\n");
            for (int j = 0; j < 9; j++) {
                if (j == 3 || j == 6) printf("| ");
                printf("%d ", sudoku[i][j]);
            }
            printf("\n");
        }
        return;
    }

    // find S heuristic (column with fewest nodes)
    struct node *col;
    int smallest = 10, size;
    for (struct node *c = matrix[0].right; c != &matrix[0]; c = c->right) {
        size = 0;
        for (struct node *r = c->down; r != c; r = r->down) {
            size++;
        }
        if (size < smallest) {
            smallest = size;
            col = c;
        }
    }

    // go through column and cover each row in it one after another
    cover(col);
    for (struct node *r = col->down; r != col; r = r->down) {
        for (struct node *c = r->right; c != r; c = c->right) {
            cover(c->header);
        }
        sudoku[r->row][r->col] = r->num;
        // contine with the row as part of the solution
        solve();
        // after all possibilities with that row are explored, undo and try with the next row
        for (struct node *c = r->left; c != r; c = c->left) {
            uncover(c->header);
        }
    }
    uncover(col);
}

int main () {
    // create 2d matrix
    struct node2d matrix2d[9*9*9][9*9*4];
    int it = (9*9*4)+1;
    for (int i = 0; i < 9*9*9; i++) {
        for (int j = 0; j < 9*9*4; j++) {
            matrix2d[i][j].filled = false;
            matrix2d[i][j].pointer = NULL;
        }
        int arr[4] = {i/9, (9*9)+(i%9)+((i/81)*9), (9*9*2)+(i%81), (9*9*3)+(i%9)+(((i%(9*9))/(9*3))*9)+((i/(9*9*3))*9*3)}; // took forever
        for (int j = 0; j < 4; j++) {
            matrix2d[i][arr[j]].filled = true;
            matrix2d[i][arr[j]].pointer = &matrix[it];

            // assign info to linked matrix nodes
            matrix[it].num = (i%9)+1;
            matrix[it].row = i/81;
            matrix[it].col = (i/9)%9;
            it++;
        }
    }

    // create linked matrix headers
    matrix[0].left = &matrix[9*9*4];
    for (int i = 1; i < (9*9*4)+1; i++) {
        matrix[i].up = &matrix[i];
        matrix[i].down = &matrix[i];
        matrix[i].left = &matrix[i-1];
        matrix[i-1].right = &matrix[i];
    }
    matrix[9*9*4].right = &matrix[0];

    // assign linked matrix pointers based off of 2d matrix
    struct node *leftNode;
    for (int i = 0; i < 9*9*9; i++) {
        for (int j = 0; j < 9*9*4; j++) {
            if (matrix2d[i][j].filled) {
                // header
                matrix2d[i][j].pointer->header = &matrix[j+1];

                // up
                for (int n = i; n >= 0; n --) {
                    if (matrix2d[n][j].filled && n != i) {
                        matrix2d[i][j].pointer->up = matrix2d[n][j].pointer;
                        break;
                    } else if (n == 0) {
                        matrix2d[i][j].pointer->up = &matrix[j+1];
                        matrix[j+1].down = matrix2d[i][j].pointer;
                    }
                }

                // down
                for (int n = i; n < 9*9*9; n++) {
                    if (matrix2d[n][j].filled && n != i) {
                        matrix2d[i][j].pointer->down = matrix2d[n][j].pointer;
                        break;
                    } else if (n+1 == 9*9*9) {
                        matrix2d[i][j].pointer->down = &matrix[j+1];
                        matrix[j+1].up = matrix2d[i][j].pointer;
                    }
                }

                // left
                struct node *leftNode;
                for (int n = j; n >= 0; n --) {
                    if (matrix2d[i][n].filled && n != j) {
                        matrix2d[i][j].pointer->left = matrix2d[i][n].pointer;
                        break;
                    } else if (n == 0) {
                        leftNode = matrix2d[i][j].pointer;
                    }
                }

                // right
                for (int n = j; n < 9*9*4; n++) {
                    if (matrix2d[i][n].filled && n != j) {
                        matrix2d[i][j].pointer->right = matrix2d[i][n].pointer;
                        break;
                    } else if (n+1 == 9*9*4) {
                        matrix2d[i][j].pointer->right = leftNode;
                        leftNode->left = matrix2d[i][j].pointer;
                    }
                }
            }
        }
    }

    // load and dislay sudoku problem
    char n;
    it = 0;
    printf("PROBLEM\npaste: ");
    while (scanf("%c", &n) && it < 80) {
        if (n == 32) n = 48;
        if (it == 3*9 || it == 6*9) printf("------+-------+------\n");
        if (it%9 == 3 || it%9 == 6) printf("| ");
        printf("%c ", (n == 48 ? ' ' : n));
        sudoku[it/9][it%9] = n-48;
        it++;
        if (it%9 == 0) printf("\n");
    }
    printf("\n");

    // cover numbers given by sudoku puzzle
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (sudoku[i][j] != 0) {
                for (int k = 0; k < 10*9*9*4+1; k++) {
                    if (matrix[k].num == sudoku[i][j] && matrix[k].row == i && matrix[k].col == j) {
                        cover(matrix[k].header);
                    }
                }
            }
        }
    }

    // solve sudoku recursively & display solutions
    solve();
    if (solutions == 0) {
        printf("NO SOLUTIONS\n");
    }
}
