/**
* File: main.c
* Author: Tyler Evans
*
* Creation Date: 9/21/21
* Last Modified: 11/10/21
* Version: 3.0
* 
* Description:
*   Contains the main program that creates and simulates a one-dimensional cellular
*   automaton with the specified command line parameters.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ca.h"

// // test for creation of 1DCA
// void test1DCA (int w, unsigned char qState) {
//     struct ca_data *data = create1DCA(w, qState);
//     initCA(data, qState);
//     displayCA(data);
// }

// // test for creation of 2DCA
// void test2DCA (int w, int h, unsigned char qState) {
//     struct ca_data *data = create2DCA(w, h, qState);
//     initCA(data, qState);
//     displayCA(data);
// }

// void testSet1DCACell (int w, unsigned char qState, int x, unsigned char state) {
//     struct ca_data *data = create1DCA(w, qState);
//     initCA(data, qState);
//     displayCA(data);
//     set1DCACell(data, x, state);
//     displayCA(data);
// }

// void testSet2DCACell (int w, int h, unsigned char qState, int x, int y, unsigned char state) {
//     struct ca_data *data = create2DCA(w, h, qState);
//     initCA(data, qState);
//     displayCA(data);
//     printf("\n");
//     set2DCACell(data, x, y, state);
//     displayCA(data);
// }

// void testStep1DCA () {
//     struct ca_data *data = create1DCA(5, 0);
//     initCA(data, 0);
//     set1DCACell(data, 0, 1);
//     set1DCACell(data, 3, 1);
//     displayCA(data);
//     data->wrap = 0;
//     step1DCA(data, rule110);
//     displayCA(data);
// }

// void testStep2DCA () {
//     struct ca_data *ca = create2DCA(4, 4, 0);
//     initCA(ca, 1);
//     ca->wrap = 1;
//     displayCA(ca);
//     printf("\n");
//     set2DCACell(ca, 1, 0, 0);
//     set2DCACell(ca, 3, 0, 0);
//     set2DCACell(ca, 0, 1, 0);
//     set2DCACell(ca, 2, 1, 0);
//     set2DCACell(ca, 0, 2, 0);
//     set2DCACell(ca, 3, 2, 0);
//     set2DCACell(ca, 1, 3, 0);
//     displayCA(ca);
//     step2DCA(ca, gameOfLife);
//     printf("\n");
//     displayCA(ca);
// }

/**
* Main method that executes the program. Initializes and simulates a one-dimensional cellular automaton.
* Takes 5 command line parameters as follows:
*   (1) number of cells
*   (2) number of states
*   (3) whether or not to wrap
*   (4) initial state to set the cells to
*   (5) number of steps to simulate
*/
void main (int argc, char *argv[]) {
    
    // check to make sure there are the correct number of arguments
    if (argc > 3) {
        printf("Error: Too many arguments passed");
        return;
    } else if (argc < 3) {
        printf("Error: Too few arguments passed");
        return;
    }

    // verify the dimensions
    int dimension = atoi(argv[1]);
    if (dimension != 2) {
        printf("Error: The program only supports 2 dimensional cellular automaton");
        return;
    }

    // load info from file and create 2DCA
    FILE *file = fopen(argv[2], "r");
    char buff[255];
    int height, width;
    fscanf(file, "%d", &height);
    fscanf(file, "%d", &width);
    // printf("%d, %d\n", height, width);

    struct ca_data *ca = create2DCA(width, height, 0);
    ca->wrap = 1;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int state;
            fscanf(file, "%d", &state);
            set2DCACell(ca, x, y, state);
        }
    }
    displayCA(ca);

    // run program (while loop)
    char input;
    scanf("%c", &input);
    while (input == '\n') {
        step2DCA(ca, gameOfLife);
        displayCA(ca);
        scanf("%c", &input);
    }
}