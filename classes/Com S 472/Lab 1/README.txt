How to compile and run:

In order to run the program, navigate to the directory
containing the 'eightpuzzle.jar' file in the command line
and execute the following command:

java -jar eightpuzzle.jar <Input File Path> <algorithm>

My program can run files containing multiple puzzles and
will solve every puzzle within the input file. I have
included 4 files that I used to obtain the data for my lab
report, each with a (hopefully) self-explanatory name.

My heuristic for h3 was the result of some research into
the topic, and is a sum of the Manhattan Distance and a
penalty for each directly adjacent swappable tile (i.e.
the number of tiles directly adjacent to each other who,
if swapped, would be in their proper positions in the goal
state). My implementation of this heuristic was not the
greatest or most efficient, so the results weren't as good
as I expected but still satisfactory as I explain in my
report.

NOTE: The jar file was compiled with Java 11, so it should
work on most computers with Java installed.