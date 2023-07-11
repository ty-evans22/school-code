#include <iostream>
#include "GraphicsClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;

/**
  * @brief Construct a new Graphics Client object
  * 
  * @param url - the url of the server to connect to
  * @param port - the port of the server to connec to
  */
GraphicsClient::GraphicsClient(std::string url, int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    this->port = port;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        fprintf(stderr, "Invalid address/Address not supported\n");
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Connection Failed\n");
    }

    
}

/**
 * @brief Clones a GraphicsClient object.
 * 
 * @param g1 - the GraphicsClient object to clone
 */
GraphicsClient::GraphicsClient(const GraphicsClient& g1) {
  this->sockfd = g1.sockfd;
  this->port = port;
}

void GraphicsClient::operator =(const GraphicsClient& g1) {
  this->sockfd = g1.sockfd;
  this->port = g1.port;
}

GraphicsClient::~GraphicsClient() {
    // close(sockfd);
}

/**
 * @brief Converts a color value into a hex value.
 * 
 * @param dec - the decimal value of the color
 * @param byte - the byte to return
 * @return int - the hex value to return
 */
int colorHex(int dec, int byte) {
    char hex_string[2];
    sprintf(hex_string, "%X", dec);
    // may need to right shift hex string here too
    string s = "0";
    s = s + hex_string[byte];
    return std::stoi(s, 0, 16);
}

/**
 * @brief Converts a char value into a hex value/
 * 
 * @param c - the char value to convert
 * @param byte - the byte to return
 * @return int - the hex value to return
 */
int charHex(char c, int byte) {
    char buffer[2];
    sprintf(buffer, "%X", c);
    string s = "0";
    s = s + buffer[byte];
    return std::stoi(s, 0, 16);
}

/**
 * @brief Converts a coordinate value into a hex value.
 * 
 * @param dec - the decimal value of the coordinate
 * @param byte - the byte to return
 * @return int - the hex value to return
 */
int coordHex(int dec, int byte) {
    char hex_string[4];
    int n = sprintf(hex_string, "%X", dec);
    // right shift hex string
    for (int i = 0; i < n; i++) {
        hex_string[i + 4 - n] = hex_string[i];
        hex_string[i] = 0;
    }
    string s = "0";
    s = s + hex_string[byte];
    return std::stoi(s, 0, 16);
}

/**
  * @brief Sets the background color to the specified RGB value.
  * 
  * @param red - the red value
  * @param green - the green value
  * @param blue - the blue value
  */
void GraphicsClient::setBackgroundColor(int red, int green, int blue) {
    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x00;
    message[4] = 0x07;
    message[5] = 0x02;
    message[6] = colorHex(red, 0);
    message[7] = colorHex(red, 1);
    message[8] = colorHex(green, 0);
    message[9] = colorHex(green, 1);
    message[10] = colorHex(blue, 0);
    message[11] = colorHex(blue, 1);

    send(sockfd, message, 12, 0);
}

/**
  * @brief Set the current drawing color to the specified RGB value.
  * 
  * @param red - the red value
  * @param green - the green value
  * @param blue - the blue value
  */
void GraphicsClient::setDrawingColor(int red, int green, int blue) {
    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x00;
    message[4] = 0x07;
    message[5] = 0x06;
    message[6] = colorHex(red, 0);
    message[7] = colorHex(red, 1);
    message[8] = colorHex(green, 0);
    message[9] = colorHex(green, 1);
    message[10] = colorHex(blue, 0);
    message[11] = colorHex(blue, 1);

    send(sockfd, message, 12, 0);
}

/**
 * @brief Clears the screen.
 * 
 */
void GraphicsClient::clear() {
    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x00;
    message[4] = 0x01;
    message[5] = 0x01;

    send(sockfd, message, 6, 0);
}

/**
  * @brief Sets a specified pixel to a specified RGB value.
  * 
  * @param x - the x coordinate of the pixel
  * @param y - the y coordinate of the pixel
  * @param red - the red value
  * @param green - the green value
  * @param blue - the blue value
  */
void GraphicsClient::setPixel(int x, int y, int red, int green, int blue) {
    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x00;
    message[4] = 0x0F;
    message[5] = 0x03;
    message[6] = (x >> 12)&0x0F;
    message[7] = (x >> 8)&0x0F;
    message[8] = (x >> 4)&0x0F;
    message[9] = (x)&0x0F;
    message[10] = (y >> 12)&0x0F;
    message[11] = (y >> 8)&0x0F;
    message[12] = (y >> 4)&0x0F;
    message[13] = (y)&0x0F;
    message[14] = colorHex(red, 0);
    message[15] = colorHex(red, 1);
    message[16] = colorHex(green, 0);
    message[17] = colorHex(green, 1);
    message[18] = colorHex(blue, 0);
    message[19] = colorHex(blue, 1);

    send(sockfd, message, 20, 0);
}

/**
  * @brief Draws a rectangle of a given width and height at the specified point.
  * 
  * @param x - the x coordinate to draw the rectangle at
  * @param y - the y coordinate to draw the rectangle at
  * @param width - the width of the rectangle to draw
  * @param height - the height of the rectangle to draw
  */
void GraphicsClient::drawRectangle(int x, int y, int width, int height) {
    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x01;
    message[4] = 0x01;
    message[5] = 0x07;
    message[6] = (x >> 12)&0x0F;
    message[7] = (x >> 8)&0x0F;
    message[8] = (x >> 4)&0x0F;
    message[9] = (x)&0x0F;
    message[10] = (y >> 12)&0x0F;
    message[11] = (y >> 8)&0x0F;
    message[12] = (y >> 4)&0x0F;
    message[13] = (y)&0x0F;
    message[14] = (width >> 12)&0x0F;
    message[15] = (width >> 8)&0x0F;
    message[16] = (width >> 4)&0x0F;
    message[17] = (width)&0x0F;
    message[18] = (height >> 12)&0x0F;
    message[19] = (height >> 8)&0x0F;
    message[20] = (height >> 4)&0x0F;
    message[21] = (height)&0x0F;

    send(sockfd, message, 22, 0);
}

/**
  * @brief Draws a filled rectangle of a given width and height at the specified point.
  * 
  * @param x - the x coordinate to draw the rectangle at
  * @param y - the y coordinate to draw the rectangle at
  * @param width - the width of the rectangle to draw
  * @param height - the height of the rectangle to draw
  */
void GraphicsClient::fillRectangle(int x, int y, int width, int height) {
    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x01;
    message[4] = 0x01;
    message[5] = 0x08;
    message[6] = (x >> 12)&0x0F;
    message[7] = (x >> 8)&0x0F;
    message[8] = (x >> 4)&0x0F;
    message[9] = (x)&0x0F;
    message[10] = (y >> 12)&0x0F;
    message[11] = (y >> 8)&0x0F;
    message[12] = (y >> 4)&0x0F;
    message[13] = (y)&0x0F;
    message[14] = (width >> 12)&0x0F;
    message[15] = (width >> 8)&0x0F;
    message[16] = (width >> 4)&0x0F;
    message[17] = (width)&0x0F;
    message[18] = (height >> 12)&0x0F;
    message[19] = (height >> 8)&0x0F;
    message[20] = (height >> 4)&0x0F;
    message[21] = (height)&0x0F;

    send(sockfd, message, 22, 0);
}

/**
  * @brief Clears a rectangle of a given width and height at the specified point.
  * 
  * @param x - the x coordinate to clear the rectangle at
  * @param y - the y coordinate to clear the rectangle at
  * @param width - the width of the rectangle to clear
  * @param height - the height of the rectangle to clear
  */
void GraphicsClient::clearRectangle(int x, int y, int width, int height) {
    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x01;
    message[4] = 0x01;
    message[5] = 0x09;
    message[6] = (x >> 12)&0x0F;
    message[7] = (x >> 8)&0x0F;
    message[8] = (x >> 4)&0x0F;
    message[9] = (x)&0x0F;
    message[10] = (y >> 12)&0x0F;
    message[11] = (y >> 8)&0x0F;
    message[12] = (y >> 4)&0x0F;
    message[13] = (y)&0x0F;
    message[14] = (width >> 12)&0x0F;
    message[15] = (width >> 8)&0x0F;
    message[16] = (width >> 4)&0x0F;
    message[17] = (width)&0x0F;
    message[18] = (height >> 12)&0x0F;
    message[19] = (height >> 8)&0x0F;
    message[20] = (height >> 4)&0x0F;
    message[21] = (height)&0x0F;

    send(sockfd, message, 22, 0);
}

/**
  * @brief Draws an oval of the given width and height at the specified point.
  * 
  * @param x - the x coordinate to draw the oval at
  * @param y - the y coordinate to draw the oval at
  * @param width - the width of the oval to draw
  * @param height - the height of the oval to draw
  */
void GraphicsClient::drawOval(int x, int y, int width, int height) {
    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x01;
    message[4] = 0x01;
    message[5] = 0x0A;
    message[6] = (x >> 12)&0x0F;
    message[7] = (x >> 8)&0x0F;
    message[8] = (x >> 4)&0x0F;
    message[9] = (x)&0x0F;
    message[10] = (y >> 12)&0x0F;
    message[11] = (y >> 8)&0x0F;
    message[12] = (y >> 4)&0x0F;
    message[13] = (y)&0x0F;
    message[14] = (width >> 12)&0x0F;
    message[15] = (width >> 8)&0x0F;
    message[16] = (width >> 4)&0x0F;
    message[17] = (width)&0x0F;
    message[18] = (height >> 12)&0x0F;
    message[19] = (height >> 8)&0x0F;
    message[20] = (height >> 4)&0x0F;
    message[21] = (height)&0x0F;

    send(sockfd, message, 22, 0);
}

/**
  * @brief Draws an oval of the given width and height at the specified point.
  * 
  * @param x - the x coordinate to draw the oval at
  * @param y - the y coordinate to draw the oval at
  * @param width - the width of the oval to draw
  * @param height - the height of the oval to draw
  */
void GraphicsClient::fillOval(int x, int y, int width, int height) {
    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x01;
    message[4] = 0x01;
    message[5] = 0x0B;
    message[6] = (x >> 12)&0x0F;
    message[7] = (x >> 8)&0x0F;
    message[8] = (x >> 4)&0x0F;
    message[9] = (x)&0x0F;
    message[10] = (y >> 12)&0x0F;
    message[11] = (y >> 8)&0x0F;
    message[12] = (y >> 4)&0x0F;
    message[13] = (y)&0x0F;
    message[14] = (width >> 12)&0x0F;
    message[15] = (width >> 8)&0x0F;
    message[16] = (width >> 4)&0x0F;
    message[17] = (width)&0x0F;
    message[18] = (height >> 12)&0x0F;
    message[19] = (height >> 8)&0x0F;
    message[20] = (height >> 4)&0x0F;
    message[21] = (height)&0x0F;

    send(sockfd, message, 22, 0);
}

/**
  * @brief Draws a line from one point to another.
  * 
  * @param x1 - the x coordinate of the first point
  * @param y1 - the y coordinate of the first point
  * @param x2 - the x coordinate of the second point
  * @param y2 - the y coordinate of the second point
  */
void GraphicsClient::drawLine(int x1, int y1, int x2, int y2) {
    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x01;
    message[4] = 0x01;
    message[5] = 0x0D;
    message[6] = (x1 >> 12)&0x0F;
    message[7] = (x1 >> 8)&0x0F;
    message[8] = (x1 >> 4)&0x0F;
    message[9] = (x1)&0x0F;
    message[10] = (y1 >> 12)&0x0F;
    message[11] = (y1 >> 8)&0x0F;
    message[12] = (y1 >> 4)&0x0F;
    message[13] = (y1)&0x0F;
    message[14] = (x2 >> 12)&0x0F;
    message[15] = (x2 >> 8)&0x0F;
    message[16] = (x2 >> 4)&0x0F;
    message[17] = (x2)&0x0F;
    message[18] = (y2 >> 12)&0x0F;
    message[19] = (y2 >> 8)&0x0F;
    message[20] = (y2 >> 4)&0x0F;
    message[21] = (y2)&0x0F;

    send(sockfd, message, 22, 0);
}

/**
 * @brief Draws a string to the screen at the specified position.
 * 
 * @param x - the x coordinate of the position to draw the string
 * @param y - the y coordinate of the position to draw the string
 * @param s - the string to be drawn to the screen
 */
void GraphicsClient::drawString(int x, int y, std::string s) {
    char message[14 + (s.length() * 2)];

    message[0] = 0xFF;
    message[1] = coordHex((s.length() * 2) + 9, 0);
    message[2] = coordHex((s.length() * 2) + 9, 1);
    message[3] = coordHex((s.length() * 2) + 9, 2);
    message[4] = coordHex((s.length() * 2) + 9, 3);
    message[5] = 0x05;
    message[6] = (x >> 12)&0x0F;
    message[7] = (x >> 8)&0x0F;
    message[8] = (x >> 4)&0x0F;
    message[9] = (x)&0x0F;
    message[10] = (y >> 12)&0x0F;
    message[11] = (y >> 8)&0x0F;
    message[12] = (y >> 4)&0x0F;
    message[13] = (y)&0x0F;
    for (int i = 0; i < s.length() * 2; i++) {
        message[14 + i] = charHex(s.at(i / 2), i % 2);
    }

    send(sockfd, message, 15 + (s.length() * 2), 0);
}

/**
  * @brief Updates the screen.
  * 
  */
void GraphicsClient::repaint() {
    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x00;
    message[4] = 0x01;
    message[5] = 0x0C;

    send(sockfd, message, 6, 0);

    // message[5] = 0x0C;
    // send( sockfd, message, 6, 0);
}

/**
         * @brief Gets a message from the server
         * 
         * @param count - the number of bytes to retrieve
         * @return char* - the message
         */
char* GraphicsClient::getMessage(int count) {
  static char message[300];

  read(sockfd, message, count);

  return message;
}

/**
         * @brief Gets the number of bytes available to be retrieved from the server.
         * 
         * @return int - the number of bytes
         */
int GraphicsClient::getCount() {
  int count;
  ioctl(sockfd, FIONREAD, &count);
  return count;
}