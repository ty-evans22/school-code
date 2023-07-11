### README

## Requirements Met
The program currently meets all of the required expectations. The only minor caveat is that instead of having no exits along the world border, it wraps around to the other edge. However, this was stated on Piazza as being allowed.

## Program Description
Upon running the program, the world array will be generated, and the center map will be created and displayed. Then, each map in the world will be generated as it is visited, and stored in the array for the user to come back to, should they wish.

## Key Functions
The function <int fly_to_position(...)> in [worldGen.c](worldGen.c) is what is actually used to generate new maps as they are needed, and is used even when travelling to the next map over in any of the cardinal directions as the function <int move_in_direction(...)> simply calculates the position of the next map and calls <int fly_to_position(...)>. All of the map generation is done in [mapGen.c](mapGen.c) and is identical for the most part to the instructor's posted solution, apart from a few minor changes listed in the [ChangeLog](CHANGELOG.md).

## How to Run
The program can be run by typing 'make', followed by './assignment-1.02'. The map can be traversed using inputs of 'n', 's', 'e', 'w', or 'f x y' (where x and y are coordinates to travel to). It can be exited by using the input 'q'. All other inputs will give an error. Additionally, a seed may be used to generate identical worlds every time the program is run, by typing the seed as a command line parameter.