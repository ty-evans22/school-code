# CHANGELOG

## January 31st, 2022
- started project

## February 2nd, 2022
- added function to create array representation of map
- added function to generate an exit point for each side
- added function to print map to console
- encountered a bug with array creation where array would not be created
- determined bug was a result of improperly allocating memory for the array and fixed it by properly using malloc
- added function to randomly create the path from North/South
- changed North/South function so that path generation favors straighter paths (less 'spaghetti' paths)
- adjusted path generation function to randomly create the path from both the East and West exits to the nearest point on the North/South path
- added functions to generate and place PokeMart and PokeCenter along the path
- added function to place a region of tall grass randomly on the map

## February 3rd, 2022
- added out-of-bounds checker to tall grass generation
- slightly improved issue with getting stuck in a loop so it doesn't happen as often (although it still happens quite a bit)

## February 7th, 2022
- renamed terrain.c to [mapGen.c](mapGen.c)
- updated [mapGen.c](mapGen.c) to be up to date with instructor's working code
- added [heap.c](heap.c) and [heap.h](heap.h) from instructor's working code
- created [header file](mapGen.h) for [mapGen.c](mapGen.c)
- created [main.c](main.c) file for running the main program
- modified the `new_map` function to use exit points as parameters in map creation

## February 8th, 2022
- created [worldGen.c](worldGen.c) file for generating the world
- created [header file](worldGen.h) for [worldGen.c](worldGen.c)
- added ability to generate a world represented by a 399x399 array of map structs
- added functionality to be able to move between maps
- updated map generation to include probability for generating PokeMart and PokeCenter based on distance from the center
- updated world generation to wrap around at the edges (found it to be easier than modifying the path generation to not generate paths on the edges)
- updated Makefile to include newly created files

## February 9th, 2022
- updated README
- updated documention within code

## February 20th, 2022
- added functionality to display the player character on the map as an '@'

## February 22nd, 2022
- added Dijkstra's algorithm to generate the distance maps
- added functionality to display the distance maps
- updated documentation

## February 23rd, 2022
- updated README