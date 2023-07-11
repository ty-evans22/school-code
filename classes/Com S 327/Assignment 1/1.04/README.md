### README

## Requirements Met
The program currently meets all of the required expectations. Upon running it, it displays the initial map, simulates NPC and player movement using a priority queue, and shows an updated version of the map approximately 4 times per second.

## Program Description
Upon running the program, the world array will be generated, and the center map will be created and displayed. Then, the number of each type of NPC is calculated based on the total number of NPCs, and they are placed onto the map and simulated using a priority queue.

## Key Functions
Most of the simulating is done in the main() function, and isn't nearly as clean as the rest of the program. However, it accomplishes its task, and there is also a decent amount of documentation within the code to help understand it.

## How to Run
The program can be run by typing 'make', followed by './assignment-1.04'. A seed may also be used to generate identical worlds every time the program is run, by typing the seed as a command line parameter. It also has the capability for a --numtrainers switch which can be used to control the exact number of NPCs that are spawned in. There is a hard coded limit of 100 NPCs, as I felt it got too crowded with higher amounts.