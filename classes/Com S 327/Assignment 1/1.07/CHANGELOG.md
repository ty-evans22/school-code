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

## March 8th, 2022
- cleaned up code from previous assignment for maximum efficiency
- added command line safeguards
- added functionality to generate and display NPCs and the player onto the map
- reorganized code into more specialized files for cleanliness and ease of access

## March 9th, 2022
- added priority queue to cycle through the NPCs and simulate movement
- added functionality to continuously run and simulate the NPCs while displaying the map
- updated README

## March 10th, 2022
- removed some debugging stuff that I forgot about
- cleaned up some of the NPC simulation code so that it doesn't look nearly as convoluted

## March 22nd, 2022
- implemented the ncurses library
- added manual movement functionality to PC
- reorganized code into even more specialized files to reduce clutter (also rewrote the vast majority of it because I felt it was far too messy and I threw it together sloppily just to get it to work)
- implemented some of the professor's code to increase cleanliness (he (usually) writes pretty good looking code)
- removed the --numtrainers switch because I felt that each map should have a random number of trainers (kept the seed command line argument though b/c I like using that for debugging)

## March 23rd, 2022
- fixed bug where player was only able to move right or down (was caused by trying to store negative values as an unsigned int)
- started adding messages to display at the top of the screen
- added trainer battles and placeholder interface for them
- added ability to enter/exit PokeMart and PokeCenter w/ placeholder interfaces for them as well
- added functionality to display list of trainers and scroll through if needed (although I still haven't technically tested the scrolling feature yet - I'm pretty confident in my implementation)
- began reimplementing world traversal

## March 24th, 2022
- encountered strange bug that I have not been able to reproduce where the player appeared in the trainer list (even though I specifically disclude the PC from the list - which works every other time) with absurd values for the distance - going to chalk it up as a hallucination and pretend it didn't happen unless it appears again in the future
- got world traversal working for North/South exits, going to make sure it is seamless and bug free before implementing on the other two exits
- encountered the bug again, although this time with actually reasonable values but a blank char representation and no actual trainer at that spot - seems to be some ghost trainers living in my code
- fixed bug from above, it was an issue with two nearly identical if statements that relied on each other and I had only modified one and not the other
- fully implemented world traversal for all exits
- added some color to spice things up and make it look pretty

## March 30th, 2022
- began converting all code from C to C++
- finished converting code from C to C++
- encountered a major issue with the new code where the program no longer runs properly - results in immediate SEGFAULT error
- after hours of debugging, narrowed the issue down to memory allocation/deallocation with objects in C++
- was unable to finish the assignment before the deadline

## April 6th, 2022
- implemented parts of the professor's code to catch back up
- finalized conversion from C to C++, including parsing of database files
- fixed issue with displaying color for trainers
- added extra button mapping for player movement (w, e, r, s, d, f, x, c, v)
- discovered a few minor bugs (NPCs able to initiate battles while the PC is on a Pokemon Center or a PokeMart, NPCs able to walk on Pokemon Centers or PokeMarts, issue with player/trainers disappearing when the player skips a turn)
- fixed the bugs listed above
- added spawning for Pokemon
- updated move selection for Pokemon to require the minimum level listed for the move
- fixed display of Pokemon info
- ran into bug when attempting to implement stats based on pokemon level - stats are way too low
- updated and improved the output displayed upon a wild pokemon encounter - very useful for debugging
- rechecked the math for the stats and realized it was working correctly the whole time and it was my math that was wrong