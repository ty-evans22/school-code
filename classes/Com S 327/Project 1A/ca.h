/**
* File: ca.h
* Author: Tyler Evans
*
* Creation Date: 10/7/21
* Last Modified: 11/10/21
* Version: 2.0
* 
* Description:
*   Contains all of the prototypes for the functions used to create and simulate a cellular automaton.
*/

#ifndef CA_H_
#define CA_H_

/**
* Structure representation that contains all of the information needed to represent a cellular automaton.
*/
struct ca_data {
    unsigned char (*cadata)[];    // pointer to beginning of CA array (either 1 or 2 dimensional)
    unsigned char dimension;  // flag that contains 1 if 1DCA or 2 if 2DCA
    unsigned char wrap;       // flag that contrains 0 if no wrap and 1 if wrap
    unsigned char qState;
    int width;                // gives width of CA
    int height;               // gives height of CA
    int numStates;
};

/**
 * Function to initialize the cellular automaton.
 * 
 * @param ca - pointer to the structure containing the cellular automaton representation
 * @param state - the initial state to set all of the cells to
 */
void initCA (struct ca_data *ca, int state);

/**
* Function to change the state of a specified cell in a one-dimensional cellular automaton.
*
* @param ca - pointer to the structure containing the cellular automaton representaion
* @param x - the index of the cell whose state is to be changed
* @param state - the new state to set the cell to
*/
void set1DCACell (struct ca_data *ca, unsigned int x, unsigned char state);

/**
 * Function to change the state of a specified cell in a two-dimensional cellular automaton.
 * 
 * @param ca - pointer to the structure containing the cellular automaton representation
 * @param x - the x coordinate of the cell whose state is to be changed
 * @param y - the y coordinate of the cell whose state is to be changed
 * @param state - the new state to set the cell to
 */
void set2DCACell (struct ca_data *ca, unsigned int x, unsigned int y, unsigned char state);

/**
 * Function to textually display a cellular automaton.
 * 
 * @param ca - pointer to the structure containing the cellular automaton representation
 */
void displayCA(struct ca_data *ca);

/**
 * Function to create, but not initialize a one-dimensional cellular automaton.
 * 
 * @param w - number of cells in the 1DCA
 * @param qState - the quiscent state to set the cells to
 */
struct ca_data *create1DCA (int w, unsigned char qState);

/**
 * Function to create, but not initialize a two-dimensional cellular automaton.
 * 
 * @param w - the width of the 2DCA
 * @param h - the height of the 2DCA
 * @param qState - the quiscent state to set the cells to
 */
struct ca_data *create2DCA (int w, int h, unsigned char qState);

/**
 * Function to simulate a step on the one-dimensional cellular automaton.
 * 
 * @param ca - pointer to the structure containing the cellular automaton representation
 * @param rule - pointer to a function that defines a rule that dictates how a step is simulated
 */
void step1DCA (struct ca_data *ca, unsigned char (*rule) (struct ca_data *, int));

/**
 * Function to simulate a step on the two-dimensional cellular automaton.
 * 
 * @param ca - pointer to the stucture containing the cellular automaton representation
 * @param rule - pointer to a function that defines a rule that dictates how a step is simulated
 */
void step2DCA (struct ca_data *ca, unsigned char (*rule) (struct ca_data *, int, int));

/**
 * Function defining the rule 110 algorithm.
 * 
 * @param ca - pointer to the structure containing the cellular automaton representation
 * @param index - the index of the current cell that is being simulated
 */
unsigned char rule110(struct ca_data *ca, int index);

/**
 * Function that defines the game of life algorithm. Used as a rule for the step simulation.
 * 
 * @param ca - pointer to the structure containing the cellular automaton representation
 * @param x - the x coordinate of the current cell to find the next state of
 * @param y - the y coordinate of the current cell to find the next state of
 */
unsigned char gameOfLife(struct ca_data *ca, int x, int y);

#endif