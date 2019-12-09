#include <stdio.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr 
 
 //fd_set is a set of sockets to "monitor" for some activity


//FD_ZERO - Clear an fd_set
//FD_ISSET - Check if a descriptor is in an fd_set
//FD_SET - Add a descriptor to an fd_set
//FD_CLR - Remove a descriptor from an fd_set


//int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

//FD_SET(int fd, fd_set *fdset);

//FD_CLR(int fd, fd_set *fdset);

//FD_ISSET(int fd, fd_set *fdset);

//FD_ZERO(fd_set *fdset);

#include <cstdio>
#include<string>
#include <cstring>  //strlen
#include <cstdlib>

#include <cerrno>
#include <unistd.h>  //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>  //FD_SET ,FD_ISSET,FD_MACROS


#define TRUE   1
#define FALSE  0
#define PORT 8888



int main(int argc ,char* argv[])
{
    int opt=TRUE;
    int master_socket;
    int addrlen;
    int new_socket;
    int client_socket[30];
    int max_clients = 30 ;
    int  activity;
    int i ;
    int  valread ;
    int sd;
    int max_sd;
    struct sockaddr_in address; //socket where address to be installed

    char buffer[1025];

    fd_set readfds;// set of socket descriptors
    char * message="Echo Daemon v1.0\r\n";

    for(auto & iter:client_socket){
        *iter=0;
    }


    //create a master socket

    if((master_socket=socket(AF_INET,SOCK_STREAM,0))==0)
    {
        perror("socket failed");
        exit(EXI_FAILURE);
    }

    //set master socket to allow multiple connections , this is just a good habit, it will work without this

    //For example, suppose we want to set the socket option to reuse the address to 1 (on/true), we pass in the "level" SOL_SOCKET and the value we want it set to.

        //int value = 1;    
        //setsockopt(mysocket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
        //This will set the SO_REUSEADDR in my socket to 1.
    //se master socket options to reuse address for multiple connections
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );



    if(bind(master_socket,(struct sockaddr*)&address,sizeof(address) ) <0 ){
        perror("bind failed");
        exit(EXIT_FAILURE);
    
       }

    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket

    if(listen(master_socket,3)<0)
    {
        perror("listern");
        exit(EXIT_FAILURE);
    }
    addrlen=sizeof(address);

    puts("Waiting for the Connections ...");
    while(TRUE){

        //clear the  socket set
        FD_ZERO(&readfds);

        //addd master socket to set 
        FD_SET(master_socket,&readfds);

        max_sd=master_socket;

        //add child sockets to set 

        for (i=0;i<max_clients;++i)
        {
            //socket desriptor
            sd=client_socket[i];

            //if valid 
        }



    }


}