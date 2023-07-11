#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "mapGen.h"
#include "worldGen.h"

int main(int argc, char *argv[]) {
    struct timeval tv;
    uint32_t seed;

    if (argc == 2) {
        seed = atoi(argv[1]);
    } else {
        gettimeofday(&tv, NULL);
        seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
    }

    printf("Using seed: %u\n", seed);
    srand(seed);

    /* Generate the initial map (center) */
    int curX = 199;
    int curY = 199;
    map_t *w[WORLD_Y][WORLD_X];
    new_world(w);
    print_curr_map(w, curX, curY);

    /* Keeps taking input from the user until the user quits */
    int quit = 0;
    while (quit == 0) {
        char input[9];
        scanf("%s", input);
        char c = input[0];
        int x, y;
        switch (c) {
            case 'n':
                if (move_in_direction(w, north, curX, curY) == 0) {
                    curY--;
                    print_curr_map(w, curX, curY);
                }
                break;
            case 's':
                if (move_in_direction(w, south, curX, curY) == 0) {
                    curY++;
                    print_curr_map(w, curX, curY);
                }
                break;
            case 'e':
                if (move_in_direction(w, east, curX, curY) == 0) {
                    curX++;
                    print_curr_map(w, curX, curY);
                }
                break;
            case 'w':
                if (move_in_direction(w, west, curX, curY) == 0) {
                    curX--;
                    print_curr_map(w, curX, curY);
                }
                break;
            case 'q':
                quit = 1;
                break;
            case 'f':
                scanf("%d", &x);
                scanf("%d", &y);
                x += 199;
                y += 199;
                if (fly_to_position(w, x, y) == 0) {
                    curX = x;
                    curY = y;
                    print_curr_map(w, curX, curY);
                }
                break;
            default:
                printf("ERROR: Invalid input.");
        }
    }

    return 0;
}