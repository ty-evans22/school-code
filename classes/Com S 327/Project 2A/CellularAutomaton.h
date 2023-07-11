#include "GraphicsClient.h"

#ifndef CELLULARAUTOMATON_H
#define CELLULARAUTOMATON_H

class CellularAutomaton {
    private:

        /**
         * @brief The width of the CA
         * 
         */
        int width;

        /**
         * @brief The height of the CA
         * 
         */
        int height;

        /**
         * @brief The quiscent state of the CA
         * 
         */
        int qState;

        /**
         * @brief The data stored in the CA
         * 
         */
        int** cadata;

    public:

        /**
         * @brief Construct a new Cellular Automaton object
         * 
         * @param fileName - the file to get the data from
         * @param qState - the quiscent state of the CA
         */
        CellularAutomaton(std::string fileName, int qState);

        CellularAutomaton(const CellularAutomaton& c1);

        ~CellularAutomaton();

        void operator =(const CellularAutomaton& c1);

        /**
         * @brief Conducts a step through the CA
         * 
         * @param rule - the rule function to follow that defines a single step
         */
        void step2DCA(int (*rule)(CellularAutomaton, int, int));

        /**
         * @brief Displays the CA onto the server
         * 
         * @param g - the GraphicsClient used to connect to the server
         */
        void display2DCA(GraphicsClient g);

        /**
         * @brief Gets the width of the CA
         * 
         * @return int - the width
         */
        int getWidth();

        /**
         * @brief Gets the height of the CA
         * 
         * @return int - the height
         */
        int getHeight();

        /**
         * @brief Gets the quiscent state of the CA
         * 
         * @return int - the quiscent state
         */
        int getQState();

        /**
         * @brief Randomizes a cell in the CA
         * 
         * @param x - the x value of the cell
         * @param y - the y value of the cell
         */
        void randomizeCell(int x, int y);

        /**
         * @brief Gets the data of the CA
         * 
         * @return int** - the two dimensional array containing the CA data
         */
        int** getCAData();
};

#endif