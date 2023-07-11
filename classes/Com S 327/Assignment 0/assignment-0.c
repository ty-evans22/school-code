#include <stdio.h>
#include <stdbool.h>

// these arrays contain all of the possible movements for a knight (8 total)
int row[] = {2, 1, -1, -2, -2, -1, 1, 2, 2};
int col[] = {1, 2, 2, 1, -1, -2, -2, -1, 1};

// check that the given coordinates are a valid position
bool isValid(int x, int y) {
    if (x < 0 || y < 0 || x >= 5 || y >= 5) {
        return false;
    }
    return true;
}

// recursive function to perform the knight's tour using backtracking
void knightTour(int visited[5][5], int x, int y, int pos) {
    // marks the current square as visited
    visited[x][y] = pos;

    // if we have visited all squares then print the solution
    if (pos >= 25) {
        // print each square in the order it was visited
        for (int v = 1; v <= 25; v++) {
            // find which square has the current position value (v)
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    if (visited[i][j] == v) {
                        printf("%d", (i * 5) + j + 1);
                        if (v != 25) {
                            printf(", ");
                        }
                        break;
                    }
                }
            }
        }
        printf("\n");

        // backtrack before returning
        visited[x][y] = 0;
        return;
    }

    // check all possible movements and recur if valid
    for (int m = 0; m < 8; m++) {
        // get the next position
        int nextX = x + row[m];
        int nextY = y + col[m];

        if (isValid(nextX, nextY) && visited[nextX][nextY] == 0) {
            knightTour(visited, nextX, nextY, pos + 1);
        }
    }

    // backtrack from the current square and remove it from the current path
    visited[x][y] = 0;
}

int main() {
    // visited array stores the order in which squares are visited
    int visited[5][5] = {0};

    int pos = 1;

    // start knight's tour from each possible starting spot by iterating through
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            knightTour(visited, i, j, pos);
        }
    }

    return 0;
}