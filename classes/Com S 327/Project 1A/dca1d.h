/**
* File: dca1d.h
* Author: Tyler Evans
*
* Creation Date: 9/21/21
* Last Modified: 9/22/21
* Version: 1.1
* 
* Description:
*   Contains all of the prototypes for the functions used to initialize, modify, and display
*   the one-dimensional cellular automaton.
*/

#ifndef DCA1D_H_
#define DCA1D_H_

/**
* Function to initialize a one-dimensional cellular automaton to the quiscent, all-zero state.
*
* @param numCells - the number of cells to initialize
* @param dca1D - pointer to the start of the array representing the one-dimensional cellular automaton
*/
void init1DCA (int numCells, unsigned char *dca1D);

/**
* Function to set the specified cell to the specified state.
*
* @param dca1D - pointer to the start of the array representing the one-dimensional cellular automaton
* @param index - int value representing the index of the cell in the array to change the state of
* @param newState - the new state to set the cell to
*/
void set1DCACell (unsigned char *dca1D, int index, unsigned char newState);

/**
* Function to output the current state of the one-dimensional cellular automaton.
*
* @param dca1D - pointer to the start of the array representing the one-dimensional cellular automaton
* @param numCells - the number of cells within the one-dimensional cellular automaton
*/
void display1DCA (unsigned char *dca1D, int numCells);

#endif