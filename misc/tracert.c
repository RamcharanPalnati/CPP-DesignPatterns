#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<pthread.h>
#include<poll.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<signal.h>
#include<sys/sem.h>
#include<poll.h>
#include<pthread.h>
#include<sys/select.h>
#include<sys/un.h>
#define SA (struct sockaddr*)


#include<netinet/ip.h>
#include<netinet/ip_icmp.h>


unsigned short csum (unsigned short *buf, int nwords)
{
  unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}

int main (int argc, char *argv[])
{
  if (argc != 3)
    {
      printf ("need destination for tracert\n");
      exit (0);
    }
  int sfd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);  // IPV4 RAW SOCKETS PROTOCO : IPROTO_ICMP
  char buf[4096] = { 0 };
  struct ip *ip_hdr = (struct ip *) buf;
  int hop = 0;
  int one = 1;
  const int *val = &one;// conts int * val which has address of one 
  if (setsockopt (sfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0) //include IP_HEADERS in socket
    printf ("Cannot set HDRINCL!\n"); 

  struct sockaddr_in addr;
  addr.sin_port = htons (7);
  addr.sin_family = AF_INET;
  inet_pton (AF_INET, argv[2], &(addr.sin_addr));// convert IPV4 addres from text to Binary

  while (hop<30)
  {
      ip_hdr->ip_hl = 5;//ip header length 
      ip_hdr->ip_v = 4; //ip version 
      ip_hdr->ip_tos = 0;  //ip type pf service 
      ip_hdr->ip_len = 20 + 8; //ip total length 
      ip_hdr->ip_id = 10000; //ip _identification 
      ip_hdr->ip_off = 0; // fragement offset field 
      ip_hdr->ip_ttl = hop; //ip _ttl 
      ip_hdr->ip_p = IPPROTO_ICMP; //ip_protocol  
      inet_pton(AF_INET, argv[1], &(ip_hdr->ip_src)); //ip_src
      inet_pton(AF_INET, argv[2], &(ip_hdr->ip_dst));//ip_destination 
      ip_hdr->ip_sum = csum((unsigned short *)buf, 9); //ip_header sum 

      struct icmphdr *icmphd = (struct icmphdr *)(buf + 20);
      icmphd->type = ICMP_ECHO;
      icmphd->code = 0;
      icmphd->checksum = 0;
      icmphd->un.echo.id = 0;
      icmphd->un.echo.sequence = hop + 1;
      icmphd->checksum = csum((unsigned short *)(buf + 20), 4);
      //sendto (sfd, buf, 28, 0, SA & addr, sizeof addr);
      sendto(sfd, buf, sizeof(struct ip) + sizeof(struct icmphdr), 0, SA & addr, sizeof addr);
      char buff[4096] = {0};
      struct sockaddr_in addr2;
      socklen_t len = sizeof(struct sockaddr_in);
      //recvfrom (sfd, buff, 28, 0, SA & addr2, &len);
      recvfrom(sfd, buff, sizeof(buff), 0, SA & addr2, &len);
      struct icmphdr *icmphd2 = (struct icmphdr *)(buff + 20);
      if (icmphd2->type != 0)
          printf("hop limit:%d Address:%s\n", hop, inet_ntoa(addr2.sin_addr));
      else
      {
          printf("Reached destination:%s with hop limit:%d\n",
                 inet_ntoa(addr2.sin_addr), hop);
          exit(0);
      }

      hop++;
    }

  return 0;
}





//inet_pton (AF_INET, "192.168.1.168", &(ip_hdr->ip_src));
