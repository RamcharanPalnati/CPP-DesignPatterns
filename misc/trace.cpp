
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/sem.h>
#include <poll.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/un.h>
#define SA (struct sockaddr *)
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string>
#include <iostream>
#include <netdb.h>
//#include <ctime>
#include <time.h>
#include<sys/time.h>

#define MAXHOPS 30

using namespace std;

unsigned short csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}


typedef struct icmp_hdr    
{
    unsigned char  type;             // ICMP Error type
    unsigned char  code;             // Type sub code
    unsigned short checksum;
    unsigned short id;
    unsigned short seq;
} ICMP_HDR;


const  int FALLBACK_TO_ICMP    = 1;       // Send ICMP echo requests if UDP fails
int    DESTINATION_UNREACHABLE = 0;
int    MAX_CONQ_ICMP_TIMEOUT   = 2;       // Maximum consequtive ICMP hops that are allowed to tuneout
int    latency;

// Structure for IPV4 header
typedef struct _IPHeader
{
    unsigned char  ip_header_len:4;  // 4-bit header length (in 32-bit words) normally=5 (Means 20 Bytes may be 24 also)
    unsigned char  ip_version   :4;  // 4-bit IPv4 version
    unsigned char  ip_tos;           // IP type of service
    unsigned short ip_total_length;  // Total length
    unsigned short ip_id;            // Unique identifier
	unsigned short ip_frag_offset;   // Fragment offset field
	unsigned char  ip_ttl;           // Time to live
    unsigned char  ip_protocol;      // Protocol(TCP,UDP,ICMP etc)
    unsigned short ip_checksum;      // IP checksum
    unsigned int   ip_srcaddr;       // Source address
    unsigned int   ip_destaddr;      // Destination address
} IPHeader;
//  Returns 1 on reaching destination, else returns 0

int main(int argc,char **argv)
{

    try
    {
        int socketFileDescriptor = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
        if(socketFileDescriptor==0){
            cout<<"Unable to create Socket error Socket Error"<<endl;
            return 1;
        }        
        char bufferReadSocket[1024];
        //Form IP Header
        struct ip *ip_hdr = (struct ip *)bufferReadSocket;//IP HEADER
        int numberOfHops=0;

        int one=1;
        const int* val=&one;
        if(setsockopt(socketFileDescriptor,IPPROTO_IP,IP_HDRINCL,val,sizeof(one)) < 0 ){
            cout<<"cannot set HDRINCL !"<<endl;
            return 1;
        }


        
        struct addrinfo * dnsLookUpIP;//query through dnsLookup getaddrinfo
        struct sockaddr_in *destinationSockAddr;//
        string destinationIpAddStr;
        

        int retOfDnsQuery = getaddrinfo(argv[1], NULL, NULL, &dnsLookUpIP);

        if(retOfDnsQuery!=0){// !=0 false case
            cout<<"DNS LOOK UP query error:"<<gai_strerror(retOfDnsQuery)<<endl;
            return 1;
        }


        if(dnsLookUpIP!=NULL){
                        
            destinationSockAddr=(struct sockaddr_in *)dnsLookUpIP->ai_addr;
            char destinationIpAddress[INET_ADDRSTRLEN];
            strcpy(destinationIpAddress,inet_ntoa(destinationSockAddr->sin_addr));
            destinationIpAddStr=destinationIpAddress;//convert destinationIpAddrStr into c++ string 
            cout<<"Traceroute to "<<argv[1]<<" ("<<inet_ntoa(destinationSockAddr->sin_addr)<<") with max 30 hops\n";

        }else{
            cout<<"DNS LOOKUP IS NULL"<<endl;
            return 1;
        }
        destinationSockAddr->sin_port=htons(5484);
        destinationSockAddr->sin_family=AF_INET;        


        while(numberOfHops<MAXHOPS){

            ip_hdr->ip_hl=5;  // in words size is always 20bytes for IPV4
            ip_hdr->ip_v=4; // IP version
            ip_hdr->ip_tos=0; //ip type of service ....best service 
            ip_hdr->ip_len=20+8;
            ip_hdr->ip_id=10000; //id of ip recognisation 
            ip_hdr->ip_off=0; //fragment offset 
            ip_hdr->ip_ttl=numberOfHops;
            ip_hdr->ip_p=IPPROTO_ICMP;
            inet_pton(AF_INET,"0.0.0.0",&(ip_hdr->ip_src));
            inet_pton(AF_INET,argv[1],&(ip_hdr->ip_dst));
            ip_hdr->ip_sum=csum((unsigned short *)bufferReadSocket,9);//caliuclate checksum

            struct icmphdr *icmphd=(struct icmphdr*)(bufferReadSocket+20);
            icmphd->type=ICMP_ECHO;
            icmphd->code=0;
            icmphd->checksum=0;
            icmphd->un.echo.id=0;
            icmphd->un.echo.sequence=numberOfHops+1;
            icmphd->checksum=csum((unsigned short*)(bufferReadSocket+20),4);
            
            
            struct timeval tvStart;
            struct timeval tvEnd;
            

            memset(&tvEnd, 0, sizeof(tvEnd));
            memset(&tvStart, 0, sizeof(tvStart));

            
            gettimeofday(&tvStart,NULL);
            int sendRetVal=sendto(socketFileDescriptor,bufferReadSocket,sizeof(struct ip)+sizeof(struct icmphdr),0,(struct sockaddr *)  destinationSockAddr,sizeof *destinationSockAddr );
            char receiveBuff[4096]={0};

            if(sendRetVal==-1){

                cout<<"Error in sending message to IP ;"<<errno<<endl;
                return 1;
            }
            socklen_t socketLength=sizeof(struct sockaddr_in);
            int recvRetVal=recvfrom(socketFileDescriptor,receiveBuff,sizeof(receiveBuff),0,0,0);
            gettimeofday(&tvEnd,NULL);

            float totalTimeTaken=(float)(((1000000 * (tvEnd.tv_sec - tvStart.tv_sec)) + (tvEnd.tv_usec - tvStart.tv_usec)) / 1000.00);
                        

            if(recvRetVal>0){
                

                //ProcessPacket(char* Buffer, int size, unsigned long dest)
                struct ip * ip_hdr2=(struct ip*)(receiveBuff);

                struct icmphdr *icmphd2 = (struct icmphdr *)(receiveBuff + 20);
                //if(ip_hdr2->ip_pr)
                
                struct sockaddr_in sockAddr;
                memset(&sockAddr,0,sizeof(sockAddr));
                sockAddr.sin_addr=ip_hdr2->ip_src;

                

                bool destinationReachable=false;
                if (icmphd2->type == 3)
                {
                    cout << "Destination Unreachable" << endl;
                    destinationReachable=true;
                }
                else if (icmphd2->type == 11)
                {
                    cout << "TTL Expired" << endl;
                }
                else if (icmphd2->type == 0)
                {
                    cout << "ICMP Echo Reply" << endl;
                }

                if((!destinationReachable)|| (sockAddr.sin_addr.s_addr==destinationSockAddr->sin_addr.s_addr)){
                    printf("\n %2d  %f ms  %15s \n",numberOfHops, totalTimeTaken, inet_ntoa(sockAddr.sin_addr));
                }
                if(sockAddr.sin_addr.s_addr==destinationSockAddr->sin_addr.s_addr){
                    cout<<"Reached the Destination "<<inet_ntoa(sockAddr.sin_addr)<<endl;
                    break;
                }

            }else{
                cout<<"Recv from failed  error :errno:"<<errno<<endl;
            }
            

            ++numberOfHops;
        }

        close(socketFileDescriptor);
        return 0;


    }
    catch (exception &e)
    {

        cout << "Error in traceroute" << e.what() << endl;
    }
}