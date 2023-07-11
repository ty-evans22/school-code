/**
* File: dca1d.c
* Author: Tyler Evans
*
* Creation Date: 9/21/21
* Last Modified: 9/22/21
* Version: 1.1
* 
* Description:
*   Contains definitions for all of the functions used to initialize, modify, and display
*   the one-dimensional cellular automaton.
*/

#include <stdio.h>

/**
* Function that cycles through each cell within the array representing the one-dimensional cellular automaton
* and sets the cell to 0.
*
* @param numCells - the number of cells to initialize
* @param dca1D - pointer to the start of the array representing the one-dimensional cellular automaton
*/
void init1DCA (int numCells, unsigned char *dca1D) {
    for (int i = 0; i < numCells; i++) {
        dca1D[i] = 0;
    }
}

/**
* Function that sets the specified cell to the specified state. Additionally, validates the input (currently only checks
* if the index is negative).
*
* @param dca1D - pointer to the start of the array representing the one-dimensional cellular automaton
* @param index - int value representing the index of the cell in the array to change the state of
* @param newState - the new state to set the cell to
*/
void set1DCACell (unsigned char *dca1D, int index, unsigned char newState) {
    if (index < 0) {
        printf("Error: Index cannot be negative");
        return;
    }
    dca1D[index] = newState;
}

/**
* Function that prints each cells state, separated by a space and terminated by a newline.
*
* @param dca1D - pointer to the start of the array representing the one-dimensional cellular automaton
* @param numCells - the number of cells within the one-dimensional cellular automaton
*/
void display1DCA (unsigned char *dca1D, int numCells) {
    for (int i = 0; i < numCells - 1; i++) {
        printf("%d ", dca1D[i]);
    }
    printf("%d\n", dca1D[numCells - 1]);
}