#include <iostream>
#include <fstream>
#include "CellularAutomaton.h"
#include "GraphicsClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

/**
         * @brief Construct a new Cellular Automaton object
         * 
         * @param fileName - the file to get the data from
         * @param qState - the quiscent state of the CA
         */
CellularAutomaton::CellularAutomaton(std::string fileName, int qState) {
    this->qState = qState;
    
    ifstream file(fileName);
    // get data from file - read int by int
    file >> width;
    file >> height;

    cadata = new int*[height];
    for (int i = 0; i < height; i++) {
        // declare a memory block of size heights
        cadata[i] = new int[width];
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            file >> cadata[i][j];
        }
    }

    file.close();

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cout << cadata[i][j] << " ";
        }
        cout << "\n";
    }
}

CellularAutomaton::CellularAutomaton(const CellularAutomaton& c1) {
    this->width = c1.width;
    this->height = c1.height;
    this->qState = c1.qState;
    this->cadata = new int*[height];
    for (int i = 0; i < height; i++) {
        cadata[i] = new int[width];
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cadata[i][j] = c1.cadata[i][j];
        }
    }
}

CellularAutomaton::~CellularAutomaton() {
    // delete cadata
    // for (int i = 0; i < width; i++) {
    //     delete [] cadata[i];
    // }
    // delete [] cadata;
}

void CellularAutomaton::operator =(const CellularAutomaton& c1) {
    this->width = c1.width;
    this->height = c1.height;
    this->qState = c1.qState;
    this->cadata = new int*[height];
    for (int i = 0; i < height; i++) {
        cadata[i] = new int[width];
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cadata[i][j] = c1.cadata[i][j];
        }
    }
}

/**
         * @brief Randomizes a cell in the CA
         * 
         * @param x - the x value of the cell
         * @param y - the y value of the cell
         */
void CellularAutomaton::randomizeCell(int x, int y) {
    srand(time(0));
    cadata[y][x] = rand() % 2;
}

/**
         * @brief Gets the width of the CA
         * 
         * @return int - the width
         */
int CellularAutomaton::getWidth() {
    return width;
}

/**
         * @brief Gets the height of the CA
         * 
         * @return int - the height
         */
int CellularAutomaton::getHeight() {
    return height;
}

 /**
         * @brief Gets the quiscent state of the CA
         * 
         * @return int - the quiscent state
         */
int CellularAutomaton::getQState() {
    return qState;
}

/**
         * @brief Gets the data of the CA
         * 
         * @return int** - the two dimensional array containing the CA data
         */
int** CellularAutomaton::getCAData() {
    return cadata;
}

/**
         * @brief Conducts a step through the CA
         * 
         * @param rule - the rule function to follow that defines a single step
         */
void CellularAutomaton::step2DCA(int (*rule)(CellularAutomaton, int, int)) {
    CellularAutomaton tmp(*this);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            tmp.cadata[y][x] = rule(*this, x, y);
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cadata[y][x] = tmp.cadata[y][x];
        }
    }
}

/**
         * @brief Displays the CA onto the server
         * 
         * @param g - the GraphicsClient used to connect to the server
         */
void CellularAutomaton::display2DCA(GraphicsClient g) {
    // get max value from width and height
    int max = std::max(width, height);

    // set the cell size and cell gap
    int cellSize, cellGap;
    if (max > 200) {
        cellSize = 1;
        cellGap = 0;
    } else if (max > 100) {
        cellSize = 2;
        cellGap = 1;
    } else if (max > 50) {
        cellSize = 4;
        cellGap = 1;
    } else {
        cellSize = 10;
        cellGap = 2;
    }

    g.setDrawingColor(68, 197, 83);

    // display each cell
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (cadata[y][x] != qState) {
                cout << "filling " << x << " " << y << "\n";
                g.fillRectangle((cellGap + cellSize) * x, (cellGap + cellSize) * y, cellSize, cellSize);
                g.repaint();
            }
        }
    }
}