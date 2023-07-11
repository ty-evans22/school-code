/**
* File: ca.c
* Author: Tyler Evans
*
* Creation Date: 10/7/21
* Last Modified: 11/10/21
* Version: 2.0
* 
* Description:
*   Contains all of the necessary data structures and methods to create and simulate a cellular automaton.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
* Structure representation that contains all of the information needed to represent a cellular automaton.
*/
struct ca_data {
    unsigned char *cadata;    // pointer to beginning of CA array (either 1 or 2 dimensional)
    unsigned char dimension;  // flag that contains 1 if 1DCA or 2 if 2DCA
    unsigned char wrap;       // flag that contrains 0 if no wrap and 1 if wrap
    unsigned char qState;
    int width;                // gives width of CA
    int height;               // gives height of CA
    int numStates;
};

/**
 * Initializes the cellular automaton by setting all of the cell states to the specified value. If
 * the value is -1 than all cell states are set to random numbers from 0 to the number of possible states - 1.
 * 
 * @param ca - pointer to the structure containing the cellular automaton representation
 * @param state - the initial state to set all of the cells to
 */
void initCA (struct ca_data *ca, int state) {
    if (state == -1) {
        // random numbers
        time_t t;
        srand((unsigned) time(&t));
        for (int y = 0; y < ca->height; y++) {
            for (int x = 0; x < ca->width; x++) {
                ca->cadata[x + (y * ca->width)] = rand() % (ca->numStates);
            }
        }
    } else {
        for (int y = 0; y < ca->height; y++) {
            for (int x = 0; x < ca->width; x++) {
                ca->cadata[x + (y * ca->width)] = state;
            }
        }
    }
}

/**
 * Function that sets a specified cell in the cellular automaton to a new state. Sends an error if the coordinates are negative.
 * 
 * @param ca - pointer to the structure containing the cellular automaton representaion
* @param x - the index of the cell whose state is to be changed
* @param state - the new state to set the cell to
*/
void set1DCACell (struct ca_data *ca, unsigned int x, unsigned char state) {
    if (x < 0) {
        printf("Error: Index cannot be negative");
        return;
    }
    ca->cadata[x] = state;
}

/**
 * Function that sets a specified cell in the cellular automaton to a new state. Sends an error if the coordinates are negative.
 * 
 * @param ca - pointer to the structure containing the cellular automaton representation
 * @param x - the x coordinate of the cell whose state is to be changed
 * @param y - the y coordinate of the cell whose state is to be changed
 * @param state - the new state to set the cell to
 */
void set2DCACell (struct ca_data *ca, unsigned int x, unsigned int y, unsigned char state) {
    if (x < 0) {
        printf("Error: x value cannot be negative");
        return;
    } else if (y < 0) {
        printf("Error: y value cannot be negative");
        return;
    }
    ca->cadata[x + (y * ca->width)] = state;
    // printf("\n%d", x + (y * ca->width));
}

/**
 * Function used to display the cellular automaton in readable text form.
 * 
 * @param ca - pointer to the structure containing the cellular automaton representation
 */
void displayCA(struct ca_data *ca) {
    for (int y = 0; y < ca->height; y++) {
        for (int x = 0; x < ca->width; x++) {
            printf("%d ", ca->cadata[x + (y * ca->width)]);
        }
        printf("\n");
    }
}

/**
 * Function that creates a one-dimensional cellular automaton, but does not initialize it.
 * 
 * @param w - number of cells in the 1DCA
 * @param qState - the quiscent state to set the cells to
 */
struct ca_data *create1DCA (int w, unsigned char qState) {
    struct ca_data *d;
    d = malloc(sizeof(*d) + sizeof(unsigned char) * w);
    d->cadata = (unsigned char *)malloc(w * sizeof(unsigned char));
    d->dimension = 1;
    d->qState = qState;
    d->width = w;
    d->height = 1;
    return d;
}

/**
 * Function that creates a two-dimensional cellular automaton, but does not initialize it.
 * 
 * @param w - the width of the 2DCA
 * @param h - the height of the 2DCA
 * @param qState - the quiscent state to set the cells to
 */
struct ca_data *create2DCA (int w, int h, unsigned char qState) {
    struct ca_data *d;
    d = malloc(sizeof(*d) + sizeof(unsigned char) * (w * h));
    d->cadata = (unsigned char *)malloc((w * h) * sizeof(unsigned char));
    d->dimension = 2;
    d->qState = qState;
    d->width = w;
    d->height = h;
    return d;
}

/**
 * Function that defines the game of life algorithm. Used as a rule for the step simulation.
 * 
 * @param ca - pointer to the structure containing the cellular automaton representation
 * @param x - the x coordinate of the current cell to find the next state of
 * @param y - the y coordinate of the current cell to find the next state of
 */
unsigned char gameOfLife (struct ca_data *ca, int x, int y) {
    // get values of cell at coordinates and its eight neighbors
    unsigned char topLeft, top, topRight, left, center, right, bottomLeft, bottom, bottomRight;
    int maxX = ca->width - 1;
    int maxY = ca->height - 1;
    unsigned char q = ca->qState;
    // printf("\nX: %d, Y: %d\n", x, y);
    if (x == 0 && y == 0) {
        // top left
        if (ca->wrap == 0) {
            // no wrap
            topLeft = q;
            top = q;
            topRight = q;
            left = q;
            right = ca->cadata[x + 1 + (y * ca->width)];
            bottomLeft = q;
            bottom = ca->cadata[x + (ca->width * (y + 1))];
            bottomRight = ca->cadata[x + 1 + (ca->width * (y + 1))];
        } else {
            // wrap all lefts, all tops
            topLeft = ca->cadata[maxX + (maxY * ca->width)]; // bottom right
            top = ca->cadata[maxY * ca->width]; // bottom left
            topRight = ca->cadata[1 + (maxY * ca->width)];
            left = ca->cadata[maxX]; // top right
            right = ca->cadata[x + 1 + (y * ca->width)];
            bottomLeft = ca->cadata[maxX + (ca->width * 1)];
            bottom = ca->cadata[x + (ca->width * (y + 1))];
            bottomRight = ca->cadata[x + 1 + (ca->width * (y + 1))];
        }
    } else if (x != 0 && x != maxX && y == 0) {
        // top
        if (ca->wrap == 0) {
            // no wrap
            topLeft = q;
            top = q;
            topRight = q;
            left = ca->cadata[x - 1 + (ca->width * y)];
            right = ca->cadata[x + 1 + (ca->width * y)];
            bottomLeft = ca->cadata[x - 1 + (ca->width * (y + 1))];
            bottom = ca->cadata[x + (ca->width * (y + 1))];
            bottomRight = ca->cadata[x + 1 + (ca->width * (y + 1))];
        } else {
            // wrap all tops
            topLeft = ca->cadata[x - 1 + (ca->width * maxY)];
            top = ca->cadata[x + (ca->width * maxY)];
            topRight = ca->cadata[x + 1 + (ca->width * maxY)];
            left = ca->cadata[x - 1 + (ca->width * y)];
            right = ca->cadata[x + 1 + (ca->width * y)];
            bottomLeft = ca->cadata[x - 1 + (ca->width * (y + 1))];
            bottom = ca->cadata[x + (ca->width * (y + 1))];
            bottomRight = ca->cadata[x + 1 + (ca->width * (y + 1))];
        }
    } else if (x == maxX && y == 0) {
        // top right
        if (ca->wrap == 0) {
            // no wrap
            topLeft = q;
            top = q;
            topRight = q;
            left = ca->cadata[x - 1 + (ca->width * y)];
            right = q;
            bottomLeft = ca->cadata[x - 1 + (ca->width * (y + 1))];
            bottom = ca->cadata[x + (ca->width * (y + 1))];
            bottomRight = q;
        } else {
            // wrap all tops, all rights
            topLeft = ca->cadata[x - 1 + (ca->width * maxY)];
            top = ca->cadata[x + (ca->width * maxY)];
            topRight = ca->cadata[ca->width * maxY];
            left = ca->cadata[x - 1 + (ca->width * y)];
            right = ca->cadata[0];
            bottomLeft = ca->cadata[x - 1 + (ca->width * (y + 1))];
            bottom = ca->cadata[x + (ca->width * (y + 1))];
            bottomRight = ca->cadata[ca->width];
        }
    } else if (x == 0 && y != 0 && y != maxY) {
        // left
        if (ca->wrap == 0) {
            // no wrap
            topLeft = q;
            top = ca->cadata[x + (ca->width * (y - 1))];
            topRight = ca->cadata[x + 1 + (ca->width * (y - 1))];
            left = q;
            right = ca->cadata[x + 1 + (ca->width * y)];
            bottomLeft = q;
            bottom = ca->cadata[x + (ca->width * (y + 1))];
            bottomRight = ca->cadata[x + 1 + (ca->width * (y + 1))];
        } else {
            // wrap all lefts
            topLeft = ca->cadata[maxX + (ca->width * (y-1))];
            top = ca->cadata[x + (ca->width * (y - 1))];
            topRight = ca->cadata[x + 1 + (ca->width * (y - 1))];
            left = ca->cadata[maxX + (ca->width * y)];
            right = ca->cadata[x + 1 + (ca->width * y)];
            bottomLeft = ca->cadata[maxX + (ca->width * (y + 1))];
            bottom = ca->cadata[x + (ca->width * (y + 1))];
            bottomRight = ca->cadata[x + 1 + (ca->width * (y + 1))];
        }
    } else if (x != 0 && x != maxX && y != 0 && y != maxY) {
        // middle
        topLeft = ca->cadata[x - 1 + (ca->width * (y - 1))];
        top = ca->cadata[x + (ca->width * (y - 1))];
        topRight = ca->cadata[x + 1 + (ca->width * (y - 1))];
        left = ca->cadata[x - 1 + (ca->width * y)];
        right = ca->cadata[x + 1 + (ca->width * y)];
        bottomLeft = ca->cadata[x - 1 + (ca->width * (y + 1))];
        bottom = ca->cadata[x + (ca->width * (y + 1))];
        bottomRight = ca->cadata[x + 1 + (ca->width * (y + 1))];

        // int liveCount = topLeft + top + topRight + left + right + bottomLeft + bottom + bottomRight;
        // printf("\nCount: %d\n", liveCount);
    } else if (x == maxX && y != 0 && y != maxY) {
        // right
        if (ca->wrap == 0) {
            // no wrap
            topLeft = ca->cadata[x - 1 + (ca->width * (y - 1))];
            top = ca->cadata[x + (ca->width * (y - 1))];
            topRight = q;
            left = ca->cadata[x - 1 + (ca->width * y)];
            right = q;
            bottomLeft = ca->cadata[x - 1 + (ca->width * (y + 1))];
            bottom = ca->cadata[x + (ca->width * (y + 1))];
            bottomRight = q;

            // int liveCount = topLeft + top + topRight + left + right + bottomLeft + bottom + bottomRight;
            // printf("\nCount: %d\n", liveCount);
        } else {
            // wrap all right
            topLeft = ca->cadata[x - 1 + (ca->width * (y - 1))];
            top = ca->cadata[x + (ca->width * (y - 1))];
            topRight = ca->cadata[(y - 1) * ca->width];
            left = ca->cadata[x - 1 + (ca->width * y)];
            right = ca->cadata[y * ca->width];
            bottomLeft = ca->cadata[x - 1 + (ca->width * (y + 1))];
            bottom = ca->cadata[x + (ca->width * (y + 1))];
            bottomRight = ca->cadata[(y + 1) * ca->width];
        }
    } else if (x == 0 && y == maxY) {
        // bottom left
        if (ca->wrap == 0) {
            // no wrap
            topLeft = q;
            top = ca->cadata[x + (ca->width * (y - 1))];
            topRight = ca->cadata[x + 1 + (ca->width * (y - 1))];
            left = q;
            right = ca->cadata[x + 1 + (ca->width * y)];
            bottomLeft = q;
            bottom = q;
            bottomRight = q;
        } else {
            // wrap all left, all bottom
            topLeft = ca->cadata[maxX + ((y - 1) * ca->width)];
            top = ca->cadata[x + (ca->width * (y - 1))];
            topRight = ca->cadata[x + 1 + (ca->width * (y - 1))];
            left = ca->cadata[maxX + (y * ca->width)];
            right = ca->cadata[x + 1 + (ca->width * y)];
            bottomLeft = ca->cadata[maxX];
            bottom = ca->cadata[x];
            bottomRight = ca->cadata[x + 1];
        }
    } else if (x != 0 && x != maxX && y == maxY) {
        // bottom
        if (ca->wrap == 0) {
            // no wrap
            topLeft = ca->cadata[x - 1 + (ca->width * (y - 1))];
            top = ca->cadata[x + (ca->width * (y - 1))];
            topRight = ca->cadata[x + 1 + (ca->width * (y - 1))];
            left = ca->cadata[x - 1 + (ca->width * y)];
            right = ca->cadata[x + 1 + (ca->width * y)];
            bottomLeft = q;
            bottom = q;
            bottomRight = q;
        } else {
            // wrap all bottom
            topLeft = ca->cadata[x - 1 + (ca->width * (y - 1))];
            top = ca->cadata[x + (ca->width * (y - 1))];
            topRight = ca->cadata[x + 1 + (ca->width * (y - 1))];
            left = ca->cadata[x - 1 + (ca->width * y)];
            right = ca->cadata[x + 1 + (ca->width * y)];
            bottomLeft = ca->cadata[x - 1];
            bottom = ca->cadata[x];
            bottomRight = ca->cadata[x + 1];
        }
    } else if (x == maxX && y == maxY) {
        // bottom right
        if (ca->wrap == 0) {
            // no wrap
            topLeft = ca->cadata[x - 1 + (ca->width * (y - 1))];
            top = ca->cadata[x + (ca->width * (y - 1))];
            topRight = q;
            left = ca->cadata[x - 1 + (ca->width * y)];
            right = q;
            bottomLeft = q;
            bottom = q;
            bottomRight = q;
        } else {
            // wrap all bottom, all right
            topLeft = ca->cadata[x - 1 + (ca->width * (y - 1))];
            top = ca->cadata[x + (ca->width * (y - 1))];
            topRight = ca->cadata[(y - 1) * ca->width];
            left = ca->cadata[x - 1 + (ca->width * y)];
            right = ca->cadata[ca->width * maxY];
            bottomLeft = ca->cadata[x - 1];
            bottom = ca->cadata[x];
            bottomRight = ca->cadata[0];

            // printf("Top left: %d\n", topLeft);
            // printf("Top: %d\n", top);
            // printf("Top right: %d\n", topRight);
            // printf("Left: %d\n", left);
            // printf("Right: %d\n", right);
            // printf("Bottom left: %d\n", bottomLeft);
            // printf("Bottom: %d\n", bottom);
            // printf("Bottom right: %d\n", bottomRight);
            // int liveCount = topLeft + top + topRight + left + right + bottomLeft + bottom + bottomRight;
            // printf("\nCount: %d\n", liveCount);
        }
    }

    int liveCount = topLeft + top + topRight + left + right + bottomLeft + bottom + bottomRight;

    if (ca->cadata[x + (ca->width * y)] == 0) {
        // dead
        if (liveCount == 3) {
            return 1;
        } else {
            return 0;
        }
    } else {
        // alive
        if (liveCount !=2 && liveCount != 3) {
            return 0;
        } else {
            return 1;
        }
    }
}

/**
* Function that defines the rule 110 algorithm. Used as a rule for the step simulation.
*
* @param data - pointer to the structure containing the cellular automaton representation
* @param index - the index of the current cell that is being simulated
*/
unsigned char rule110 (struct ca_data *ca, int index) {
    // get values of cell at index and its two neighbors
    unsigned char left, center, right;
    if (index == 0) {
        // left neighbor depends on wrap value
        if (ca->wrap == 0) {
            // no wrap
            left = ca->qState;
            center = ca->cadata[index];
            right = ca->cadata[index + 1];
        } else {
            // wrap
            left = ca->cadata[ca->width - 1];
            center = ca->cadata[index];
            right = ca->cadata[index + 1];
        }
    } else if (index == ca->width - 1) {
        // right neighbor depends on wrap value
        if (ca->wrap == 0) {
            // no wrap
            left = ca->cadata[index - 1];
            center = ca->cadata[index];
            right = ca->qState;
        } else {
            // wrap
            left = ca->cadata[index - 1];
            center = ca->cadata[index];
            right = ca->cadata[0];
        }
    } else {
        // all is normal
        left = ca->cadata[index - 1];
        center = ca->cadata[index];
        right = ca->cadata[index + 1];
    }

    // printf("Test: %d %d %d\n", left, center, right);

    // return new value based on these values
    if (left == 1 && center == 1 && right == 1) {
        return 0; // 111
    } else if (left == 1 && center == 1 && right == 0) {
        return 1; // 110
    } else if (left == 1 && center == 0 && right == 1) {
        return 1; // 101
    } else if (left == 1 && center == 0 && right == 0) {
        return 0; // 100
    } else if (left == 0 && center == 1 && right == 1) {
        return 1; // 011
    } else if (left == 0 && center == 1 && right == 0) {
        return 1; // 010
    } else if (left == 0 && center == 0 && right == 1) {
        return 1; // 001
    } else {
        return 0; // 000
    }
}

/**
 * Function used to simulate a single step within the one-dimensional cellular automaton. Utilizes a function that defines a rule to follow.
 * 
 * @param data - pointer to the structure containing the cellular automaton representation
 * @param rule - pointer to a function that defines a rule that dictates how a step is simulated
 */
void step1DCA(struct ca_data *ca, unsigned char (*rule) (struct ca_data *, int)) {
    struct ca_data *tmp;
    tmp = malloc(sizeof(*ca) + sizeof(unsigned char) * ca->width);
    tmp->cadata = (unsigned char *)malloc(ca->width * sizeof(unsigned char));
    tmp->width = ca->width;
    tmp->height = ca->height;
    tmp->wrap = ca->wrap;
    tmp->qState = ca->qState;
    for (int i = 0; i < ca->width; i++) {
        tmp->cadata[i] = ca->cadata[i];
    }
    for (int i = 0; i < ca->width; i++) {
        unsigned char newChar = rule(ca, i);
        set1DCACell(tmp, i, newChar);
    }
    for (int i = 0; i < ca->width; i++) {
        ca->cadata[i] = tmp->cadata[i];
    }
}

/**
 * Function used to simulate a single step within the two-dimensional cellular automaton. Utilizes a function that defines a rule to follow.
 * 
 * @param ca - pointer to the stucture containing the cellular automaton representation
 * @param rule - pointer to a function that defines a rule that dictates how a step is simulated
 */
void step2DCA(struct ca_data *ca, unsigned char (*rule) (struct ca_data *, int, int)) {
    struct ca_data *tmp;
    tmp = malloc(sizeof(*ca) + sizeof(unsigned char) * (ca->width * ca->height));
    tmp->width = ca->width;
    tmp->height = ca->height;
    tmp->wrap = ca->wrap;
    tmp->qState = ca->qState;
    for (int y = 0; y < ca->height; y++) {
        for (int x = 0; x < ca->width; x++) {
            tmp->cadata[x + (ca->width * y)] = ca->cadata[x + (ca->width * y)];
        }
    }
    for (int y = 0; y < ca->height; y++) {
        for (int x = 0; x < ca->width; x++) {
            unsigned char newChar = rule(ca, x, y);
            // printf("\nNew Char: %d", newChar);
            set2DCACell(tmp, x, y, newChar);
        }
    }
    for (int y = 0; y < ca->height; y++) {
        for (int x = 0; x < ca->width; x++) {
            ca->cadata[x + (ca->width * y)] = tmp->cadata[x + (ca->width * y)];
        }
    }
}