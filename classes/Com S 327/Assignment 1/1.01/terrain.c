#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// define various constant values that can be tweaked
#define WIDTH 80
#define HEIGHT 21
#define MAX_PATH_LENGTH_EW 150
#define MAX_POKEPLACE_DISTANCE 250
#define TALL_GRASS_RADIUS 4

// define the char values to use for each terrain object in order for the look and feel to be easily changed
#define PATH '#'
#define POKECENTER 'C'
#define POKEMART 'M'
#define TALL_GRASS ':'
#define BOULDER '%'
#define CLEARING '.'

int loopCount = 0;
int curPath = 0;

// used during path generation
const struct Position {
    int x;
    int y;
} emptyPosition = {-1, -1}; // x and y values of -1 will be considered empty

// returns an array with all of the possible moves from the specified point
struct Position* getPossibleMoves(char** map, int curX, int curY, int targX, int targY) {
    // cannot be occupied already (not '#' or '%')
    // cannot be out of bounds
    // cannot run parallel to itself
    struct Position* moves = malloc(sizeof(struct Position) * 4);
    for (int i = 0; i < 4; i++) {
        moves[i] = emptyPosition;
    }

    if ((targX == curX && targY == curY - 1) || ((curY > 1 || (targX == curX && targY == curY - 1)) && map[curY - 1][curX] != PATH && map[curY - 1][curX] != '%')) {
        if ((curX == 0 || curX == WIDTH - 1) || ((map[curY - 1][curX + 1] != PATH || (curY - 1 == targY && curX + 1 == targX))
         && (map[curY - 1][curX - 1] != PATH || (curY - 1 == targY && curX - 1 == targX)))) {
            // top valid
            for (int i = 0; i < 4; i++) {
                if (moves[i].x == -1 && moves[i].y == -1) {
                    moves[i].x = curX;
                    moves[i].y = curY - 1;
                    break;
                }
            }
        }
    }
    if ((targX == curX + 1 && targY == curY) || ((curX < WIDTH - 2 || (targX == curX + 1 && targY == curY)) && map[curY][curX + 1] != PATH && map[curY][curX + 1] != '%')) {
        if ((curY == 0 || curY == HEIGHT - 1) || ((map[curY + 1][curX + 1] != PATH || (curY + 1 == targY && curX + 1 == targX))
         && (map[curY - 1][curX + 1] != PATH || (curY - 1 == targY && curX + 1 == targX)))) {
            // right valid
            for (int i = 0; i < 4; i++) {
                if (moves[i].x == -1 && moves[i].y == -1) {
                    moves[i].x = curX + 1;
                    moves[i].y = curY;
                    break;
                }
            }
        }
    }
    if ((targX == curX - 1 && targY == curY) || ((curX > 1 || (targX == curX - 1 && targY == curY)) && map[curY][curX - 1] != PATH && map[curY][curX - 1] != '%')) {
        if ((curY == 0 || curY == HEIGHT - 1) || ((map[curY + 1][curX - 1] != PATH || (curY + 1 == targY && curX - 1 == targX))
         && (map[curY - 1][curX - 1] != PATH || (curY - 1 == targY && curX - 1 == targX)))) {
            // left valid
            for (int i = 0; i < 4; i++) {
                if (moves[i].x == -1 && moves[i].y == -1) {
                    moves[i].x = curX - 1;
                    moves[i].y = curY;
                    break;
                }
            }
        }
    }
    if ((targX == curX && targY == curY + 1) || ((curY < HEIGHT - 2 || (targX == curX && targY == curY + 1)) && map[curY + 1][curX] != PATH && map[curY + 1][curX] != '%')) {
        if ((curX == 0 || curX == WIDTH - 1) || ((map[curY + 1][curX + 1] != PATH || (curY + 1 == targY && curX + 1 == targX))
         && (map[curY + 1][curX - 1] != PATH || (curY + 1 == targY && curX - 1 == targX)))) {
            // bottom valid
            for (int i = 0; i < 4; i++) {
                if (moves[i].x == -1 && moves[i].y == -1) {
                    moves[i].x = curX;
                    moves[i].y = curY + 1;
                    break;
                }
            }
        }
    }

    // if target position is a possible move then only return that move
    bool targPosAvail = false;
    if (targX == curX && targY == curY - 1) {
        targPosAvail = true;
        moves[0].x = curX;
        moves[0].y = curY - 1;
    } else if (targX == curX + 1 && targY == curY) {
        targPosAvail = true;
        moves[0].x = curX + 1;
        moves[1].y = curY;
    } else if (targX == curX - 1 && targY == curY) {
        targPosAvail = true;
        moves[0].x = curX - 1;
        moves[0].y = curY;
    } else if (targX == curX && targY == curY + 1) {
        targPosAvail = true;
        moves[0].x = curX;
        moves[0].y = curY + 1;
    }
    if (targPosAvail) {
        for (int i = 1; i < 4; i++) {
            moves[i] = emptyPosition;
        }
    }

    return moves;
}

// clears the map, filling it with periods and placing boulders anywhere there isn't an exit
void clearMap(char** map, int northExit, int southExit, int westExit, int eastExit) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if ((x == 0 && y != westExit) || (y == 0 && x != northExit) || (x == WIDTH - 1 && y != eastExit) || (y == HEIGHT - 1 && x != southExit)) {
                map[y][x] = '%';
            }
            else {
                map[y][x] = '.';
            }
        }
    }
}

// gets the closest point on the North/South line to the West exit (if -1 values returned for x and y than no path was found on map)
struct Position getNearestPointW(char** map, int westExit) {
    struct Position target = {.x = -1, .y = -1};
    // iterate through the map starting from the West exit, getting one step further away with each iteration in a ripple-like fashion
    for (int i = 0; i < WIDTH - 2; i++) {
        for (int j = 1; j <= i + 1; j++) {
            int xVal = j;
            int yValP = westExit + (i - j + 1); // down
            int yValM = westExit - (i - j + 1); // up
            // only check the positions that are inside the boulder border
            if (yValM >= 1) {
                if (map[yValM][xVal] == PATH) {
                    target.x = xVal;
                    target.y = yValM;
                    return target;
                }
            }
            if (yValP <= HEIGHT - 2) {
                if (map[yValP][xVal] == PATH) {
                    target.x = xVal;
                    target.y = yValP;
                    return target;
                }
            }
        }
    }
    return target;
}

// same as above but going left instead of right
struct Position getNearestPointE(char** map, int eastExit) {
    struct Position target = {.x = -1, .y = -1};
    for (int i = 0; i < WIDTH - 2; i++) {
        for (int j = 1; j <= i + 1; j++) {
            int xVal = WIDTH - 1 - j;
            int yValP = eastExit + (i - j + 1); // down
            int yValM = eastExit - (i - j + 1); // up
            // only check the positions that are inside the boulder border
            if (yValM >= 1) {
                if (map[yValM][xVal] == PATH) {
                    target.x = xVal;
                    target.y = yValM;
                    return target;
                }
            }
            if (yValP <= HEIGHT - 2) {
                if (map[yValP][xVal] == PATH) {
                    target.x = xVal;
                    target.y = yValP;
                    return target;
                }
            }
        }
    }
}

struct Position getRandomPoint(char** map) {
    struct Position target = {.x = 0, .y = 0};
    while (map[target.y][target.x] != PATH) {
        target.x = (rand() % WIDTH - 3) + 1;
        target.y = (rand() % HEIGHT - 3) + 1;
    }
    return target;
}

// copies the actual map to a temporary map
void copyMap(char** ogMap, char** copyMap) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            copyMap[i][j] = ogMap[i][j];
        }
    }
}

// outputs the map to the console
void printMap(char** map) {
    printf("\n");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

// generates a path from the West exit to a random point along the North/South path
void generatePartialPath(char** map, int northExit, int southExit, int westExit, int eastExit, int startX, int startY, int targX, int targY) {
    loopCount++;
    if (loopCount > 100) {
        return;
    }
    int curX = startX;
    int curY = startY;
    int curIndex = 0;
    struct Position* solutionPath = malloc(sizeof(struct Position) * MAX_PATH_LENGTH_EW);
    for (int i = 0; i < MAX_PATH_LENGTH_EW; i++) {
        solutionPath[i] = emptyPosition;
    }

    // create temporary copy of the map to use for path finding calculations
    char** tmpMap = (char**)malloc(HEIGHT * sizeof(char*));
    for (int i = 0; i < HEIGHT; i++) {
        tmpMap[i] = (char*)malloc(WIDTH * sizeof(char));
    }
    copyMap(map, tmpMap);

    while (curX != targX || curY != targY) {
        tmpMap[curY][curX] = PATH;
        struct Position* possibleMoves = getPossibleMoves(tmpMap, curX, curY, targX, targY);
        // check if possible moves exist
        if (possibleMoves[0].x != -1 && possibleMoves[0].y != -1) {
            // add current position to the solution path
            solutionPath[curIndex].x = curX;
            solutionPath[curIndex].y = curY;
            curIndex++;
            // get the number of possible next moves
            int numPossible = 0;
            for (int i = 0; i < 4; i++) {
                if (possibleMoves[i].x != -1 && possibleMoves[i].y != -1) {
                    numPossible++;
                }
            }
            // decide which directions to favor
            int favorLR = 0; // -1 -> favor left, 0 -> no favor, 1 -> favor right
            int favorUD = 0; // -1 -> favor down, 0 -> no favor, 1 -> favor up
            if (targX > curX)
                favorLR = 1;
            else if (targX < curX)
                favorLR = -1;
            if (targY > curY)
                favorUD = -1;
            else if (targY < curY)
                favorUD = -1;
            // sort the possible moves so that the favored ones are lower
            if (numPossible > 1) {
                for (int k = 0; k < numPossible; k++) {
                    if ((possibleMoves[k].x < curX && favorLR == -1) || (possibleMoves[k].x > curX && favorLR == 1)
                     || (possibleMoves[k].y > curY && favorUD == -1) || (possibleMoves[k].y > curY && favorUD == 1)) {
                        struct Position tmp = emptyPosition;
                        tmp.x = possibleMoves[k].x;
                        tmp.y = possibleMoves[k].y;
                        for (int g = 1; g <= k; g++) {
                            possibleMoves[g].x = possibleMoves[g - 1].x;
                            possibleMoves[g].y = possibleMoves[g - 1].y;
                        }
                        possibleMoves[0].x = tmp.x;
                        possibleMoves[0].y = tmp.y;
                     }
                }
            }
            // randomly select one of the possible moves favoring the lower positioned ones and update current position
            int s = rand() % 100;
            if (numPossible == 1) {
                curX = possibleMoves[0].x;
                curY = possibleMoves[0].y;
            } else if (numPossible == 2) {
                if (s < 60) {
                    curX = possibleMoves[0].x;
                    curY = possibleMoves[0].y;
                } else {
                    curX = possibleMoves[1].x;
                    curY = possibleMoves[1].y;
                }
            } else if (numPossible == 3) {
                if (s < 60) {
                    curX = possibleMoves[0].x;
                    curY = possibleMoves[0].y;
                } else if (s < 90) {
                    curX = possibleMoves[1].x;
                    curY = possibleMoves[1].y;
                } else {
                    curX = possibleMoves[2].x;
                    curY = possibleMoves[2].y;
                }
            } else {
                if (s < 40) {
                    curX = possibleMoves[0].x;
                    curY = possibleMoves[0].y;
                } else if (s < 80) {
                    curX = possibleMoves[1].x;
                    curY = possibleMoves[1].y;
                } else if (s < 90) {
                    curX = possibleMoves[2].x;
                    curY = possibleMoves[2].y;
                } else {
                    curX = possibleMoves[3].x;
                    curY = possibleMoves[3].y;
                }
            }
            if (curIndex == MAX_PATH_LENGTH_EW && (curX != targX || curY != targY)) {
                generatePartialPath(map, northExit, southExit, westExit, eastExit, startX, startY, targX, targY);
                return;
            }
        } else {
            // check if solution path is empty or not
            if (curIndex != 0) {
                // change the current position to the last position added to the solution path (i.e. go back one step)
                curX = solutionPath[curIndex - 1].x;
                curY = solutionPath[curIndex - 1].y;
                // remove the last position from the solution path
                solutionPath[curIndex - 1] = emptyPosition;
                curIndex--;
            } else {
                // restart the path generation process
                curX = startX;
                curY = startY;
                copyMap(map, tmpMap);
            }
        }
    }

    // update the map with the solution path
    for (int i = 0; i < MAX_PATH_LENGTH_EW; i++) {
        if (solutionPath[i].x != -1 && solutionPath[i].y != -1) {
            map[solutionPath[i].y][solutionPath[i].x] = PATH;
        }
    }
    map[targY][targX] = PATH;
}

// generates the paths for the West and East exits
void generateWEPath(char** map, int northExit, int southExit, int westExit, int eastExit) {
    // get the point on the line for the West exit to target
    struct Position targetW = getNearestPointW(map, westExit);
    struct Position targetE = getNearestPointE(map, eastExit);

    // generate path from each exit to target point
    loopCount = 0;
    curPath = 1;
    while (map[westExit][0] != PATH) {
        generatePartialPath(map, northExit, southExit, westExit, eastExit, 0, westExit, targetW.x, targetW.y);
        if (map[westExit][0] != PATH)
            targetW = getRandomPoint(map);
    }
    loopCount = 0;
    curPath = 2;
    generatePartialPath(map, northExit, southExit, westExit, eastExit, WIDTH - 1, eastExit, targetE.x, targetE.y);
}

struct Position* pokePlacePosition(char** map, int curX, int curY, struct Position* visited) {
    // update visited
    for (int i = 0; i < MAX_POKEPLACE_DISTANCE; i++) {
        if (visited[i].x == -1 && visited[i].y == -1) {
            visited[i].x = curX;
            visited[i].y = curY;
        }
    }
    struct Position* positions = malloc(sizeof(struct Position) * 4);
    for (int i = 0; i < 4; i++) {
        positions[i] = emptyPosition;
    }
    
    // check each of the 4 adjacent positions
    // up
    if (curY > 0 && map[curY - 1][curX] == '.') {
        // (1)
        if (map[curY - 2][curX] == '.' && map[curY - 2][curX + 1] == '.' && map[curY - 1][curX + 1] == '.') {
            curY = curY - 1;

            positions[0].x = curX;
            positions[0].y = curY;
            positions[1].x = curX;
            positions[1].y = curY - 1;
            positions[2].x = curX + 1;
            positions[2].y = curY - 1;
            positions[3].x = curX + 1;
            positions[3].y = curY;
            return positions;
        }

        // (2)
        if (map[curY - 2][curX] == '.' && map[curY - 2][curX - 1] == '.' && map[curY - 1][curX - 1] == '.') {
            curY = curY - 1;

            positions[0].x = curX;
            positions[0].y = curY;
            positions[1].x = curX;
            positions[1].y = curY - 1;
            positions[2].x = curX - 1;
            positions[2].y = curY - 1;
            positions[3].x = curX - 1;
            positions[3].y = curY;
            return positions;
        }
    }
    // left
    if (curX > 0 && map[curY][curX - 1] == '.') {
        // (2)
        if (map[curY - 1][curX - 1] == '.' && map[curY - 1][curX - 2] == '.' && map[curY][curX - 2] == '.') {
            curX = curX - 1;

            positions[0].x = curX;
            positions[0].y = curY;
            positions[1].x = curX;
            positions[1].y = curY - 1;
            positions[2].x = curX - 1;
            positions[2].y = curY - 1;
            positions[3].x = curX - 1;
            positions[3].y = curY;
            return positions;
        }

        // (3)
        if (map[curY][curX - 2] == '.' && map[curY + 1][curX - 2] == '.' && map[curY + 1][curX - 1] == '.') {
            curX = curX - 1;

            positions[0].x = curX;
            positions[0].y = curY;
            positions[1].x = curX - 1;
            positions[1].y = curY;
            positions[2].x = curX - 1;
            positions[2].y = curY + 1;
            positions[3].x = curX;
            positions[3].y = curY + 1;
            return positions;
        }
    }
    // right
    if (curX < WIDTH - 1 && map[curY][curX + 1] == '.') {
        // (1)
        if (map[curY - 1][curX + 1] == '.' && map[curY - 1][curX + 2] == '.' && map[curY][curX + 2] == '.') {
            curX = curX + 1;

            positions[0].x = curX;
            positions[0].y = curY;
            positions[1].x = curX;
            positions[1].y = curY - 1;
            positions[2].x = curX + 1;
            positions[2].y = curY - 1;
            positions[3].x = curX + 1;
            positions[3].y = curY;
            return positions;
        }

        // (4)
        if (map[curY + 1][curX + 1] == '.' && map[curY + 1][curX + 2] == '.' && map[curY][curX + 2] == '.') {
            curX = curX + 1;

            positions[0].x = curX;
            positions[0].y = curY;
            positions[1].x = curX;
            positions[1].y = curY + 1;
            positions[2].x = curX + 1;
            positions[2].y = curY + 1;
            positions[3].x = curX + 1;
            positions[3].y = curY;
            return positions;
        }
    }
    // down
    if (curY < HEIGHT - 1 && map[curY + 1][curX] == '.') {
        // (3)
        if (map[curY + 1][curX - 1] == '.' && map[curY + 2][curX - 1] == '.' && map[curY + 2][curX] == '.') {
            curY = curY + 1;

            positions[0].x = curX;
            positions[0].y = curY;
            positions[1].x = curX - 1;
            positions[1].y = curY;
            positions[2].x = curX - 1;
            positions[2].y = curY + 1;
            positions[3].x = curX;
            positions[3].y = curY + 1;
            return positions;
        }

        // (4)
        if (map[curY + 2][curX] == '.' && map[curY + 2][curX + 1] == '.' && map[curY + 1][curX + 1] == '.') {
            curY = curY + 1;

            positions[0].x = curX;
            positions[0].y = curY;
            positions[1].x = curX;
            positions[1].y = curY + 1;
            positions[2].x = curX + 1;
            positions[2].y = curY + 1;
            positions[3].x = curX + 1;
            positions[3].y = curY;
            return positions;
        }
    }

    // if reach this point than no matches were found - move on to the next adjacent unvisited path position
    if (curY > 0 && map[curY - 1][curX] == PATH) {
        bool visitedFlag = false;
        for (int i = 0; i < MAX_POKEPLACE_DISTANCE; i++) {
            if (visited[i].x == curX && visited[i].y == curY - 1) {
                visitedFlag = true;
                break;
            }
        }
        if (!visitedFlag) {
            curY--;
            positions = pokePlacePosition(map, curX, curY, visited);
            if (positions[0].x != -1 && positions[0].y != -1) {
                return positions;
            }
        }
    }
    if (curX > 0 && map[curY][curX - 1] == PATH) {
        bool visitedFlag = false;
        for (int i = 0; i < MAX_POKEPLACE_DISTANCE; i++) {
            if (visited[i].x == curX - 1 && visited[i].y == curY) {
                visitedFlag = true;
                break;
            }
        }
        if (!visitedFlag) {
            curX--;
            positions = pokePlacePosition(map, curX, curY, visited);
            if (positions[0].x != -1 && positions[0].y != -1) {
                return positions;
            }
        }
    }
    if (curX < WIDTH - 1 && map[curY][curX + 1] == PATH) {
        bool visitedFlag = false;
        for (int i = 0; i < MAX_POKEPLACE_DISTANCE; i++) {
            if (visited[i].x == curX + 1 && visited[i].y == curY) {
                visitedFlag = true;
                break;
            }
        }
        if (!visitedFlag) {
            curX++;
            positions = pokePlacePosition(map, curX, curY, visited);
            if (positions[0].x != -1 && positions[0].y != -1) {
                return positions;
            }
        }
    }
    if (curY < HEIGHT - 1 && map[curY + 1][curX] == PATH) {
        bool visitedFlag = false;
        for (int i = 0; i < MAX_POKEPLACE_DISTANCE; i++) {
            if (visited[i].x == curX && visited[i].y == curY + 1) {
                visitedFlag = true;
                break;
            }
        }
        if (!visitedFlag) {
            curY++;
            positions = pokePlacePosition(map, curX, curY, visited);
            if (positions[0].x != -1 && positions[0].y != -1) {
                return positions;
            }
        }
    }

    // error checking if reach here and still no spot or if max distance reached
    return positions;
}

// places the PokeCenter and PokeMart on the map
void placePokePlaces(char** map, int northExit, int southExit, int westExit, int eastExit) {
    int curX = -1;
    int curY = -1;
    // pick a random exit to start with (0 = North, 1 = West, 2 = East, 3 = South)
    int e = rand() % 4;
    switch(e) {
        case 0:
            curX = northExit;
            curY = 0;
            break;
        case 1:
            curX = 0;
            curY = westExit;
            break;
        case 2:
            curX = WIDTH - 1;
            curY = eastExit;
            break;
        case 3:
            curX = southExit;
            curY = HEIGHT - 1;
            break;
    }

    struct Position* visited = malloc(sizeof(struct Position) * MAX_POKEPLACE_DISTANCE);
    for (int i = 0; i < MAX_POKEPLACE_DISTANCE; i++) {
        visited[i] = emptyPosition;
    }

    struct Position* pokecenterPosition = pokePlacePosition(map, curX, curY, visited);
    for (int i = 0; i < 4; i++) {
        map[pokecenterPosition[i].y][pokecenterPosition[i].x] = POKECENTER;
    }

    // do the same thing as above for the pokemart
    for (int i = 0; i < MAX_POKEPLACE_DISTANCE; i++) {
        visited[i] = emptyPosition;
    }
    // pick a random exit to start with (0 = North, 1 = West, 2 = East, 3 = South)
    e = rand() % 4;
    switch(e) {
        case 0:
            curX = northExit;
            curY = 0;
            break;
        case 1:
            curX = 0;
            curY = westExit;
            break;
        case 2:
            curX = WIDTH - 1;
            curY = eastExit;
            break;
        case 3:
            curX = southExit;
            curY = HEIGHT - 1;
            break;
    }

    struct Position* pokemartPosition = pokePlacePosition(map, curX, curY, visited);
    for (int i = 0; i < 4; i++) {
        map[pokemartPosition[i].y][pokemartPosition[i].x] = POKEMART;
    }
}

void placeTallGrassRegion(char** map) {
    int curX = 0;
    int curY = 0;
    // find random unoccupied spot on the map
    while (map[curY][curX] != '.') {
        curX = (rand() % WIDTH - 2) + 1;
        curY = (rand() % HEIGHT - 2) + 1;
    }
    // grow tall grass in a radius around the selected spot
    for (int i = 0; i < TALL_GRASS_RADIUS; i++) {
        for (int j = 0; j <= i + 1; j++) {
            if (curY + (i + 1 - j) > 0 && curY + (i + 1 - j) < HEIGHT - 1 && curX + j > 0
             && curX + j < WIDTH - 1 && map[curY + (i + 1 - j)][curX + j] == '.')
                map[curY + (i + 1 - j)][curX + j] = ':';
            if (curY + (i + 1 - j) > 0 && curY + (i + 1 - j) < HEIGHT - 1 && curX - j > 0
             && curX - j < WIDTH - 1 && map[curY + (i + 1 - j)][curX - j] == '.')
                map[curY + (i + 1 - j)][curX - j] = ':';
            if (curY - (i + 1 - j) > 0 && curY - (i + 1 - j) < HEIGHT - 1 && curX + j > 0
             && curX + j < WIDTH - 1 && map[curY - (i + 1 - j)][curX + j] == '.')
                map[curY - (i + 1 - j)][curX + j] = ':';
            if (curY - (i + 1 - j) > 0 && curY - (i + 1 - j) < HEIGHT - 1 && curX - j > 0
             && curX - j < WIDTH - 1 && map[curY - (i + 1 - j)][curX - j] == '.')
                map[curY - (i + 1 - j)][curX - j] = ':';
        }
    }
}

// executes all of the steps to generate the terrain randomly
char** generateTerrain(int northExit, int southExit, int westExit, int eastExit) {
    // generate 80x21 board
    char** map = (char**)malloc(HEIGHT * sizeof(char*));
    for (int i = 0; i < HEIGHT; i++) {
        map[i] = (char*)malloc(WIDTH * sizeof(char));
    }

    clearMap(map, northExit, southExit, westExit, eastExit);

    // place random paths on corresponding side (i.e North/South,etc.)
    generatePartialPath(map, northExit, southExit, westExit, eastExit, northExit, 0, southExit, HEIGHT - 1);
    // printf("Stuck on WE Path\n");
    generateWEPath(map, northExit, southExit, westExit, eastExit);

    // printMap(map);
    // printf("Stuck on PokePlaces\n");
    // place PokeCenter and PokeMart
    placePokePlaces(map, northExit, southExit, westExit, eastExit);

    // printf("Stuck on Tall Grass\n");
    // generate tall grass regions
    for (int i = 0; i < 2; i++) {
        placeTallGrassRegion(map);
    }
    
    return map;
}

// generates an exit point for the specified side, doesn't allow exits in any of the corners
int generateExitPoint(char side) {
    int p = 0;
    switch(side) {
        case 'N':
        case 'S':
                p = (rand() % (WIDTH - 2)) + 1;
            break;
        case 'W':
        case 'E':
                p = (rand() % (HEIGHT - 2)) + 1;
            break;
        default:
            printf("ERROR: Function generateExitPoint(char side) parameter 'side' must have a value of 'N', 'S', 'W', or 'E'.\n");
    }
    return p;
}

int main() {
    // initialize random number generator
    time_t t;
    srand((unsigned) time(&t));

    // generate random exit points for each side
    int northExit, southExit, westExit, eastExit;
    northExit = generateExitPoint('N');
    southExit = generateExitPoint('S');
    westExit = generateExitPoint('W');
    eastExit = generateExitPoint('E');

    // generate the map
    char** map = generateTerrain(northExit, southExit, westExit, eastExit);

    // print the map to the console
    printMap(map);

    return 0;
}