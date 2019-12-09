#include <stdio.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int i = 0;

    while (i < 1000)
    {
        int socket_desc;
        struct sockaddr_in server;
        char *message;
        char server_reply[2000];

        //Create socket

        if (socket_desc == -1)
        {
            printf("Could not create socket");
        }

        server.sin_addr.s_addr = inet_addr("0.0.0.0");
        server.sin_family = AF_INET;
        server.sin_port = htons(8888);
        

        printf("%d\n", i);
        socket_desc = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
        {
            puts("connect error");
            return 1;
        }

        //puts("Connected\n");

        //Send some data
        message = "GET / HTTP/1.1\r\n\r\n";
        if (send(socket_desc, message, strlen(message), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        /*
        puts("Data Send\n");
        //Receive a reply from the server
        
        //puts("Reply received\n");
        
        */
        if (recv(socket_desc, server_reply, 2000, 0) < 0)
        {
            puts("recv failed");
        }
        puts(server_reply);
        close(socket_desc);
        ++i;
        //}
    }

    return 0;
}