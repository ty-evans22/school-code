#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf( stderr, "Error creating socket\n");
        exit(-1);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(7777);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        fprintf(stderr, "Invalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr, "Connection Failed \n");
        return -1;
    }

    char message[100];

    message[0] = 0xFF;
    message[1] = 0x00;
    message[2] = 0x00;
    message[3] = 0x00;
    message[4] = 0x07;
    message[5] = 0x02;
    message[6] = 0x00;
    message[7] = 0x05;
    message[8] = 0x0F;
    message[9] = 0x0F;
    message[10] = 0x00;
    message[11] = 0x00;

    send(sockfd, message, 12, 0);

    message[4] = 0x01;
    message[5] = 0x01;
    send(sockfd, message, 6, 0);

    message[5] = 0x0C;
    send( sockfd, message, 6, 0);


    close(sockfd);

    return EXIT_SUCCESS;
    
    GraphicsClient g = new GraphicsClient("127.0.0.1", 7777);
}