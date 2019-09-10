
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>

#include<arpa/inet.h>

#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/ip_icmp.h>

#include<sys/socket.h>
#include<sys/select.h>
#include<sys/un.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/sem.h>

#include<pthread.h>
#include<poll.h>
#include<stdio.h>

#include<sys/ipc.h>
#include<fcntl.h>

#include<poll.h>

unsigned short csum (unsigned short *buf, int nwords)
{
  unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}


int main(int argc, char * argv[]){
    
    if(argc!=3){
      printf ("need destination for tracert\n");
      exit (0);
    }

    int sfd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);  // IPV4 RAW SOCKETS PROTOCO : IPROTO_ICMP
    char buffer[4096]={0};

    struct ip *ip_hdr=(struct ip*)buffer;


    int hopCounter=0;


}

