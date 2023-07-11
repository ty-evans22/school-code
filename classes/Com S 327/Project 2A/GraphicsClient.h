#ifndef GRAPHICSCLIENT_H
#define GRAPHICSCLIENT_H

/**
 * @brief Contains the applicable methods needed to connect and interact with the graphics server.
 * 
 */
class GraphicsClient {
    private:

        /**
         * @brief Stores the socket info needed to connect to the server.
         * 
         */
        int sockfd;

        /**
         * @brief Stores the port info needed to connect to the server.
         * 
         */
        int port;

    public:
        /**
         * @brief Construct a new Graphics Client object
         * 
         * @param url - the url of the server to connect to
         * @param port - the port of the server to connec to
         */
        GraphicsClient(std::string url, int port);

        /**
         * @brief Clones an existing Graphics Client object
         * 
         * @param g1 - the GraphicsClient object to clone.
         */
        GraphicsClient(const GraphicsClient& g1);

        ~GraphicsClient();

        void operator =(const GraphicsClient& g1);

        /**
         * @brief Sets the background color to the specified RGB value.
         * 
         * @param red - the red value
         * @param green - the green value
         * @param blue - the blue value
         */
        void setBackgroundColor(int red, int green, int blue);

        /**
         * @brief Set the current drawing color to the specified RGB value.
         * 
         * @param red - the red value
         * @param green - the green value
         * @param blue - the blue value
         */
        void setDrawingColor(int red, int green, int blue);

        /**
         * @brief Clears the screen.
         * 
         */
        void clear();

        /**
         * @brief Sets a specified pixel to a specified RGB value.
         * 
         * @param x - the x coordinate of the pixel
         * @param y - the y coordinate of the pixel
         * @param red - the red value
         * @param green - the green value
         * @param blue - the blue value
         */
        void setPixel(int x, int y, int red, int green, int blue);

        /**
         * @brief Draws a rectangle of a given width and height at the specified point.
         * 
         * @param x - the x coordinate to draw the rectangle at
         * @param y - the y coordinate to draw the rectangle at
         * @param width - the width of the rectangle to draw
         * @param height - the height of the rectangle to draw
         */
        void drawRectangle(int x, int y, int width, int height);

        /**
         * @brief Draws a filled rectangle of a given width and height at the specified point.
         * 
         * @param x - the x coordinate to draw the rectangle at
         * @param y - the y coordinate to draw the rectangle at
         * @param width - the width of the rectangle to draw
         * @param height - the height of the rectangle to draw
         */
        void fillRectangle(int x, int y, int width, int height);

        /**
         * @brief Clears a rectangle of a given width and height at the specified point.
         * 
         * @param x - the x coordinate to clear the rectangle at
         * @param y - the y coordinate to clear the rectangle at
         * @param width - the width of the rectangle to clear
         * @param height - the height of the rectangle to clear
         */
        void clearRectangle(int x, int y, int width, int height);

        /**
         * @brief Draws an oval of the given width and height at the specified point.
         * 
         * @param x - the x coordinate to draw the oval at
         * @param y - the y coordinate to draw the oval at
         * @param width - the width of the oval to draw
         * @param height - the height of the oval to draw
         */
        void drawOval(int x, int y, int width, int height);

        /**
         * @brief Draws a filled oval of the given width and height at the specified point.
         * 
         * @param x - the x coordinate to draw the oval at
         * @param y - the y coordinate to draw the oval at
         * @param width - the width of the oval to draw
         * @param height - the height of the oval to draw
         */
        void fillOval(int x, int y, int width, int height);

        /**
         * @brief Draws a line from one point to another.
         * 
         * @param x1 - the x coordinate of the first point
         * @param y1 - the y coordinate of the first point
         * @param x2 - the x coordinate of the second point
         * @param y2 - the y coordinate of the second point
         */
        void drawLine(int x1, int y1, int x2, int y2);

        /**
         * @brief Draws a string of text at the given point.
         * 
         * @param x - the x coordinate to draw the string at
         * @param y - the y coordinate to draw the string at
         * @param s - the text to draw
         */
        void drawString(int x, int y, std::string s);

        /**
         * @brief Gets a message from the server
         * 
         * @param count - the number of bytes to retrieve
         * @return char* - the message
         */
        char* getMessage(int count);

        /**
         * @brief Gets the number of bytes available to be retrieved from the server.
         * 
         * @return int - the number of bytes
         */
        int getCount();

        /**
         * @brief Updates the screen.
         * 
         */
        void repaint();
};

#endif