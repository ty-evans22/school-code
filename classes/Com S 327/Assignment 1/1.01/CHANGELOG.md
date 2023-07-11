CHANGELOG

January 31st, 2022
- started project

February 2nd, 2022
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

February 3rd, 2022
- added out-of-bounds checker to tall grass generation
- slightly improved issue with getting stuck in a loop so it doesn't happen as often (although it still happens quite a bit)