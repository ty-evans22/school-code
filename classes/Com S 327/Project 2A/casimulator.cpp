#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "GraphicsClient.h"
#include "CellularAutomaton.h"

using namespace std;

int gol(CellularAutomaton c, int x, int y) {
    bool alive = c.getCAData()[y][x] != c.getQState();
    int count = 0;
    int width = c.getWidth();
    int height = c.getHeight();

    int north = (height + y - 1) % height;
    int south = (height + y + 1) % height;
    int east = (width + x + 1) % width;
    int west = (width + x - 1) % width;

    count = c.getCAData()[north][east] + c.getCAData()[north][west] +
            c.getCAData()[south][east] + c.getCAData()[south][west] +
            c.getCAData()[south][x] + c.getCAData()[north][x] +
            c.getCAData()[y][east] + c.getCAData()[y][west];

    if (alive) {
        if (count == 2 || count == 3) {
            return 1;
        } else {
            return c.getQState();
        }
    } else {
        if (count == 3) {
            return 1;
        } else {
            return c.getQState();
        }
    }
}

int main() { 
    GraphicsClient g("127.0.0.1", 5193);
    CellularAutomaton j("test.txt", 0);
    bool pause = true;
    bool quit = false;

    // set up the gui
    g.setDrawingColor(255, 255, 255);
    g.drawLine(600, 0, 600, 600);
    g.drawRectangle(610, 10, 180, 50);
    g.drawRectangle(610, 70, 180, 50);
    g.drawRectangle(610, 130, 180, 50);
    g.drawRectangle(610, 190, 180, 50);
    g.drawRectangle(610, 250, 180, 50);
    g.drawRectangle(610, 310, 180, 50);
    g.drawRectangle(610, 370, 180, 50);
    g.drawRectangle(610, 430, 180, 50);

    g.drawString(680, 40, "STEP");
    g.drawString(680, 100, "RUN");
    g.drawString(680, 160, "PAUSE");
    g.drawString(680, 220, "RESET");
    g.drawString(680, 280, "RANDOM");
    g.drawString(680, 340, "LOAD");
    g.drawString(680, 400, "CLEAR");
    g.drawString(680, 460, "QUIT");

    // need to add different size functions

    g.repaint();

    j.display2DCA(g);


    while(!quit) {
        if (!pause) {
            struct timespec tim;
            tim.tv_sec = 0;
            tim.tv_nsec = 100000000;
            nanosleep(&tim, &tim);

            j.step2DCA(gol);
            g.clearRectangle(0, 0, 600, 600);
            j.display2DCA(g);
            g.repaint();
        }
        

        // only run when a message is received
        if (g.getCount() > 0) {
            char* message;
            message = g.getMessage(g.getCount());
            // only run on mouse click (ignores releases)
            if (message[5] + 0x00 == 1) {
                // get the x value of the mouse click
                int x = ((message[7] + 0x00) * 4096) + ((message[8] + 0x00) * 256) + ((message[9] + 0x00) * 16) + (message[10] + 0x00);
                // get the y value of the mouse click
                int y = ((message[11] + 0x00) * 4096) + ((message[12] + 0x00) * 256) + ((message[13] + 0x00) * 16) + (message[14] + 0x00);

                // determine if a button was clicked
                if (x > 610 && x < 790) {
                    // check y values
                    if (y > 10 && y < 60) {
                        // STEP
                        cout << "STEP" << "\n";
                        j.step2DCA(gol);
                        g.clearRectangle(0, 0, 600, 600);
                        j.display2DCA(g);
                        g.repaint();
                    } else if (y > 70 && y < 120) {
                        // RUN
                        cout << "RUN" << "\n";
                        pause = false;
                    } else if (y > 130 && y < 180) {
                        // PAUSE
                        cout << "PAUSE" << "\n";
                        pause = true;
                    } else if (y > 190 && y < 240) {
                        // RESET
                        cout << "RESET" << "\n";
                        g.clearRectangle(0, 0, 600, 600);
                        g.repaint();
                    } else if (y > 250 && y < 300) {
                        // RANDOM
                        cout << "RANDOM" << "\n";
                        for (int y = 0; y < j.getHeight(); y++) {
                            for (int x = 0; x < j.getWidth(); x++) {
                                j.randomizeCell(x, y);
                            }
                        }
                        j.display2DCA(g);
                        g.repaint();
                    } else if (y > 310 && y < 360) {
                        // LOAD
                        cout << "LOAD" << "\n";
                    } else if (y > 370 && y < 420) {
                        // CLEAR
                        cout << "CLEAR" << "\n";
                        g.clearRectangle(0, 0, 600, 600);
                        g.repaint();
                    } else if (y > 430 && y < 480) {
                        // QUIT
                        cout << "QUIT" << "\n";
                        quit = true;
                    }
                }
                cout << "X: " << x << "\n";
                cout << "Y: " << y << "\n";
                for (int i = 0; i < 15; i++) {
                cout << message[i] + 0x00 << " ";
            }
            cout << "\n";
            }
        }
    }
}