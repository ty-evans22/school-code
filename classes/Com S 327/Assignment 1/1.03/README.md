### README

## Requirements Met
The program currently meets all of the required expectations. It displays both the hiker and rival distance maps, as well as the original map.

## Program Description
Upon running the program, the world array will be generated, and the center map will be created and displayed. Then, the distance maps for the hiker and the rival are calculated and displayed on the console.

## Key Functions
The function <void dijkstra(...)> in [main.c](main.c) is what is actually used to generate the distance maps, and is based off of the professor's path finding code from the first assignment.

## How to Run
The program can be run by typing 'make', followed by './assignment-1.03'. Additionally, a seed may be used to generate identical worlds every time the program is run, by typing the seed as a command line parameter.