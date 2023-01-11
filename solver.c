/*
Millisecond Sudoku Solver
by Simon Chase
*/

#include<stdio.h>
#include<stdbool.h>
#include<time.h>

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
        printf("\n");
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

int main() {
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
    FILE* f = fopen("problem.txt", "r");
    printf("PROBLEM:\n");
    int x = 0, y = 0;
    while (true) {
        n = fgetc(f);
        if (y == 9) {
            break;
        } else if (x == 9) {
            x = 0;
            y++;
            if (y%3 == 0 && y < 9) {
                printf("\n------+-------+------\n");
            } else {
                printf("\n");
            }
        } else {
            if (x%3 == 0 && x > 0) printf("| ");
            printf("%c ", n);
            if (n == '.') {
                sudoku[x][y] = 0;
            } else {
                sudoku[x][y] = n-48;
            }
            x++;
        }
    }
    printf("\n");

    clock_t begin = clock();

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
    } else {
        printf("END OF SOLUTIONS\n");
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("TIME ELAPSED: %f\n", time_spent);
}
