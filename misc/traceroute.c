/* Traceroute using raw udp packets with ICMP as failover
   By Vinod Vijayan 
   
	 Updation: 
	 15 May 07 Changed the hardcoded hostname(left by mistake) by calling gethostname.
	 16 May 07 Changed the return value on error condition to 1
	           Added check for error in sending packets that may be caused by permission issues(for raw socket) in XP.
			   Changed title from "Raw UDP Traceroute with ICMP as failover" to  
			   "Traceroute - Using RAW Socket & UDP".
			   Changed name of structure packetinfo to traceinfo

 */
#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>

// PseudoHeader for calculation of UDP checksum
typedef struct _PseudoHeader  
{
	unsigned int	SourceAddress;
	unsigned int	DestinationAddress;
	unsigned char	Zeros;
	unsigned char	Proto;
	unsigned short	Length;
} PseudoHeader;

typedef PseudoHeader FAR * LPPseudoHeader;

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

// Structure for ICMP header
typedef struct icmp_hdr    
{
    unsigned char  type;             // ICMP Error type
    unsigned char  code;             // Type sub code
    unsigned short checksum;
    unsigned short id;
    unsigned short seq;
} ICMP_HDR;

// Structure for UDP header
typedef struct udp_hdr
{
	unsigned short source_port;      // Source port no.
	unsigned short dest_port;        // Dest. port no.
	unsigned short udp_length;       // Udp packet length
	unsigned short udp_checksum;     // Udp checksum (optional)
} UDP_HDR;

// Structure for UDP payload packet
typedef struct udppacket
{
	UDP_HDR  udp;                    // UDP header
	char     messg[35];              // message
} UDP_PACKET;

// Structure for ICMP payload packet
typedef struct icmppacket
{
	ICMP_HDR icmp;                   // ICMP header
	DWORD	dwTime;                  // time
	char	cData[35];               // message
} ICMP_PACKET;

// Structure for storing information regarding a hop trace
typedef struct traceinfo
{
	int packetid;
	int ttl;
	int proto;
	int size;
	unsigned long saddr;
	unsigned long daddr;
} TRACE_INFO;


static int ICMP_SEQNO;
const  int UDP_DEST_PORTNO     = 33434;   // 33434 to 33534
const  int FALLBACK_TO_ICMP    = 1;       // Send ICMP echo requests if UDP fails
int    DESTINATION_UNREACHABLE = 0;
int    MAX_CONQ_ICMP_TIMEOUT   = 2;       // Maximum consequtive ICMP hops that are allowed to tuneout
int    latency;

// Generic code to calculate the checksum of a packet
unsigned short CalculateChecksum(unsigned short *usBuf, int iSize)
{
	unsigned long usChksum=0;
	//Calculate the checksum
	while (iSize>1)
	{
		usChksum+=*usBuf++;
		iSize-=sizeof(unsigned short);
	}

	//If we have one char left
	if (iSize)
		usChksum+=*(unsigned char*)usBuf;

	//Complete the calculations
	usChksum=(usChksum >> 16) + (usChksum & 0xffff);
	usChksum+=(usChksum >> 16);

	//Return the value (inversed)
	return (unsigned short)(~usChksum);
}

// Calculate UDP checksum using pseudo header
unsigned short CalculatePseudoChecksum(char *buf, int BufLength,unsigned long  dest,unsigned long  src)
{
	//Calculate the checksum
	LPPseudoHeader lpPseudo;
	int PseudoHeaderLength = sizeof(PseudoHeader);

	lpPseudo                     = new PseudoHeader;
	lpPseudo->DestinationAddress = dest;
	lpPseudo->SourceAddress      = src;
	lpPseudo->Zeros              = 0;
	lpPseudo->Proto              = 0x11;
	lpPseudo->Length             = htons(BufLength);

	//Calculate checksum of all
	int iTotalLength;
	iTotalLength                 = PseudoHeaderLength+BufLength;

	char* tmpBuf;
	tmpBuf=new char[iTotalLength];

	//Copy pseudo
	memcpy(tmpBuf,lpPseudo,PseudoHeaderLength);

	//Copy header
	memcpy(tmpBuf+PseudoHeaderLength,buf,BufLength);

	//Calculate the checksum
	unsigned short usChecksum;
	usChecksum                   = CalculateChecksum((unsigned short*)tmpBuf,iTotalLength);

	//Delete all
	delete tmpBuf;
	delete lpPseudo;

	//Return checksum
	return usChecksum;
}

// DNS lookup to get the IP address of the destination or host
int gethostaddr( char* pszHost,sockaddr_in& dest)
{
    // Initialize the destination host info block
    memset( &dest, 0, sizeof( dest ) );

    // Turn first passed parameter into an IP address to ping
    unsigned int addr = inet_addr( pszHost );
    //if its quad Address then OK
    if( addr != INADDR_NONE )
    {
        // It was a dotted quad number, so save result
        dest.sin_addr.s_addr	= addr;
        dest.sin_family			= AF_INET;

    }
    else
	{
        // Not in dotted quad form, so try and look it up
        hostent* hp = gethostbyname( pszHost );
        if ( hp != 0 )
		{
            // Found an address for that host, so save it
            memcpy( &(dest.sin_addr), hp->h_addr, hp->h_length );
            dest.sin_family = hp->h_addrtype;
        }
        else
		{
            // Not a recognized hostname either!
            printf("Failed to resolve %s\n", pszHost);
            return -1;
        }
    }

    return 0;
}

// If the socket is readible then there is a response which can be read.
// If not readible then after timeout reurn no response status
int IsSocketReadible(SOCKET &s)
{
	struct timeval Timeout;
	fd_set readfds;

	readfds.fd_count = 1;
	readfds.fd_array[0] = s;
	Timeout.tv_sec = 1;
    Timeout.tv_usec = 0;

	return(select(1, &readfds, NULL, NULL, &Timeout));
}

//  Returns 1 on reaching destination, else returns 0
int ProcessIpPacket (IPHeader *iphdr, unsigned long dest)
{
	unsigned short iphdrlen;
	sockaddr_in source;
	iphdrlen = iphdr->ip_header_len*4;
	static int no_hops;
	
	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iphdr->ip_srcaddr;
	// Destination unreachable is sent by the host that just previously sent an
	// ICMP time exceeded so don't print its information again
    if(!DESTINATION_UNREACHABLE || (source.sin_addr.s_addr == dest))
	printf("\n %2d  %4d ms  %15s \n", ++no_hops, latency, inet_ntoa(source.sin_addr));
	
    /* Another way is to check for ICMP echo reply but this would only be sent 
	   for ICMP echo requests */
	if(source.sin_addr.s_addr == dest)   // Destination reached 
		return(1);

	return(0);
}

// Process the ICMP response. Extract the IP header and other information
int ProcessIcmpPacket(char* Buffer , int Size, unsigned long dest)
{
	unsigned short iphdrlen;
	int dest_reached = -1;	
	IPHeader *iphdr  = (IPHeader *)Buffer;
	iphdrlen         = iphdr->ip_header_len*4;
    ICMP_HDR *icmpheader=(ICMP_HDR*)(Buffer+iphdrlen);

	if((unsigned int)(icmpheader->type)==11)
		/*printf("  (TTL Expired)\n")*/;
	else if((unsigned int)(icmpheader->type)==0) 
		/*printf("  (ICMP Echo Reply)\n")*/;
	else if((unsigned int)(icmpheader->type)==3)
		DESTINATION_UNREACHABLE = 1;

	dest_reached     = ProcessIpPacket(iphdr,dest);
	
	return dest_reached;
}

// Find the response protocol
int ProcessPacket(char* Buffer, int size, unsigned long dest)
{
	IPHeader *ip     = (IPHeader *)Buffer;
	int dest_reached = -1;

	switch (ip->ip_protocol) //Check the Protocol and do accordingly...
	{
		case 1:  //ICMP Protocol
			dest_reached = ProcessIcmpPacket(Buffer,size,dest);
			break;
		
		case 2:  //IGMP Protocol
			break;
		
		case 6:  //TCP Protocol
			break;
		
		case 17: //UDP Protocol
			break;
		
		default: //Some Other Protocol like ARP etc.
			break;
	}

	return dest_reached;
	
}

// Receive response from a readible socket and process it
int processResponse(SOCKET &s, unsigned long dest)
{
	char Buffer[1500];
	int  mangobyte;
	int  dest_reached = -1;
	
	if (Buffer == NULL)
	{
		printf("malloc() failed.\n");
		return dest_reached;
	}

	 mangobyte = recvfrom(s,Buffer,sizeof(Buffer),0,0,0); 

	if(mangobyte > 0)	
		dest_reached = ProcessPacket(Buffer, mangobyte, dest);
	else 
	{
		printf( "recvfrom() failed.\n");
	}
	return dest_reached;
}


/*void getSourceDestAddr(u16 *src_addr,u16 *dest_addr,sockaddr_in dest,sockaddr_in src)
{
    int i=0;

	src_addr[i]  =  src.sin_addr.S_un.S_un_b.s_b1;
	dest_addr[i++] = dest.sin_addr.S_un.S_un_b.s_b1;

	src_addr[i]  =  src.sin_addr.S_un.S_un_b.s_b2;
	dest_addr[i++] = dest.sin_addr.S_un.S_un_b.s_b2;

	src_addr[i]  =  src.sin_addr.S_un.S_un_b.s_b3;
	dest_addr[i++] = dest.sin_addr.S_un.S_un_b.s_b3;

	src_addr[i]  =  src.sin_addr.S_un.S_un_b.s_b4;
	dest_addr[i] = dest.sin_addr.S_un.S_un_b.s_b4;

}*/

// Construct the IP header for the trace
void construct_ip_hdr(char *pack_data,TRACE_INFO trace)
{
	//Clear the data buffer
	memset(pack_data,0,trace.size);
	int size = sizeof(pack_data);

	IPHeader *ip           = (IPHeader *)(pack_data);

	ip->ip_version         = 4;
	ip->ip_header_len      = 5;     // in words (size is always 20 bytes for IPv4)
	ip->ip_tos             = 0x0;
	ip->ip_total_length    = htons(trace.size);
	ip->ip_frag_offset     = 0x0;
	ip->ip_protocol        = trace.proto; 
	ip->ip_destaddr        = trace.daddr;
	ip->ip_srcaddr         = trace.saddr;
	ip->ip_checksum        = 0;    // Don't worry, Kernel will do the needful
	ip->ip_id              = htons(trace.packetid);
	ip->ip_ttl             = trace.ttl;
}


/*
  First construct the IP header.
  Then construct the protocol packet
*/
int construct_proto_pack(char *packdata,TRACE_INFO trace)
{
  IPHeader *ip        = (IPHeader *)(packdata); 
  int packet_length   = -1;
  static portincr     = 0;

  construct_ip_hdr(packdata,trace);

  if(trace.proto == IPPROTO_UDP)
  {
	  //struct udp_hdr *udp           = (struct udp_hdr *)(ip + 1);
	  if(portincr > 100)       // 33434 to 33534
		  portincr = 0;

      UDP_PACKET *udp_pack          = (UDP_PACKET *) (ip + 1);

      strcpy(udp_pack->messg, "Hello there!");
	  udp_pack->udp.dest_port       = htons(UDP_DEST_PORTNO + portincr++);
	  udp_pack->udp.source_port     = htons(0);
	  udp_pack->udp.udp_checksum    = 0; 
	  udp_pack->udp.udp_length      = htons(sizeof(*udp_pack));

	  // UDP Packet length = UDP header len + data len
	  int udp_hdr_n_data_len        = sizeof(*udp_pack);
	  packet_length                 = ((int)(ip + 1)  - (int)ip)    + udp_hdr_n_data_len;

	  ip->ip_total_length           = packet_length;

	  udp_pack->udp.udp_checksum    = CalculatePseudoChecksum((char *)udp_pack,udp_hdr_n_data_len,trace.daddr,trace.saddr);
  }
  else if(trace.proto == IPPROTO_ICMP)
  {
	  //struct icmp_hdr *icmp         = (struct icmp_hdr *)(ip + 1);
	  ICMP_PACKET     *icmp_pack    = (ICMP_PACKET *) (ip + 1);

	  icmp_pack->icmp.code          = 0;
	  icmp_pack->icmp.type          = 8;
	  icmp_pack->icmp.checksum      = 0;
	  icmp_pack->icmp.id            = ICMP_SEQNO;
	  icmp_pack->icmp.seq           = ICMP_SEQNO++;
      icmp_pack->dwTime             = GetTickCount();
      strcpy(icmp_pack->cData, "Hello there!");

	  // ICMP Packet length = ICMP header len + data len
	  int icmp_hdr_n_data_len       = sizeof(*icmp_pack);
      packet_length                 = ((int)(ip + 1)  - (int)ip)     + icmp_hdr_n_data_len;

	  ip->ip_total_length           = packet_length;

	  icmp_pack->icmp.checksum      = CalculateChecksum((USHORT *)icmp_pack,icmp_hdr_n_data_len);
  }
  else
	  return (-1);   // Protocol not supported

  return(packet_length);
}

// Populate the packinfo variable with parameters for a particular hop
void settraceinfo(TRACE_INFO &traceinfo,int packetid,int ttl,int proto,unsigned long dest, unsigned long src, int size)
{
	traceinfo.packetid = packetid; 
	traceinfo.ttl      = ttl;
	traceinfo.proto    = proto;
	traceinfo.saddr    = src;
	traceinfo.daddr    = dest;
	traceinfo.size     = size;
}

int main(int argc, char* argv[])
{
  sockaddr_in dest, src;
  TRACE_INFO traceinfo;
  char data[100];
  char hostname[70];

  ICMP_SEQNO      = 1;
  
  // Start Winsock up
  WSAData wsaData;

  int char_size = sizeof(unsigned char);

  if(!(argc == 2))
  {
	  printf("Usage: %s %s \n",argv[0],"target_name");
	  return 1;
  }

  if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0) 
  {
     printf("Failed to find Winsock 2.1 or better");
     return 1;
  }
	
  // Create a raw IP socket so that it can send/capture UDP and ICMP packets
  SOCKET sock = socket(AF_INET, SOCK_RAW,IPPROTO_IP);
  

  if(sock == INVALID_SOCKET) {
   printf("\nCouldn't create socket");
   return 1;
  }

  // provide our own IP header and not let the kernel provide one  

  int on = 1;
  if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (const char *)&on, sizeof(on)) == SOCKET_ERROR) {
    printf("\nCouldn't set IP_HDRINCL");
    return 1;
  }

  if(gethostaddr(argv[1],dest) < 0)     // destination domain name or IP address
  {
	printf("\n Couldn't resolve host %s \n", argv[1]);
    return 1;
  }

   //Retrive the local hostname
  if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) 
  {
     printf("\nCannot retrieve hostname");
	 return 1;
  }


  if(gethostaddr(hostname,src) < 0)               // Hostname  or IP address
  {
    return 1;
  }

  int packetid           = GetCurrentProcessId();
  int written            = 0;
  int proto              = IPPROTO_UDP;    // UDP ping
  int pack_size          = sizeof(data);
  int sockreadible       = 0;
  int dest_reached       = 0;
  int count              = 0;
  int timed_out_cnt      = 0;
  ULONG RequestTime      = 0;
  ULONG ResponseTime     = 0;

  printf("\n Tracing route to %s [%s] \n", argv[1], inet_ntoa(dest.sin_addr));
  
  while(dest_reached == 0)
  {
	  settraceinfo(traceinfo, packetid + count, count+1, proto, dest.sin_addr.s_addr, src.sin_addr.s_addr, pack_size);
	  if((pack_size = construct_proto_pack(data, traceinfo)) > 0)
	  {
		  RequestTime      = GetTickCount();
		  written          = sendto(sock,data,pack_size,0,(struct sockaddr *)&dest,sizeof(dest));

		  if (written == SOCKET_ERROR) 
		  {
			  printf("\n Sending packet failed. Check permissions on this system");
			  printf("\n Admin rights are required on XP");
			  return 1;
		  }

		  sockreadible     = IsSocketReadible(sock);
		  
		  if(sockreadible)
		  {
			  timed_out_cnt = 0;
			  ResponseTime  = GetTickCount();
			  latency       = ResponseTime - RequestTime;
			  dest_reached  = processResponse(sock,dest.sin_addr.s_addr);

			  // If destination is unreachable then switch to ICMP
			  if((DESTINATION_UNREACHABLE && (proto != IPPROTO_ICMP) && FALLBACK_TO_ICMP)) {
				  proto                   = IPPROTO_ICMP;
				  DESTINATION_UNREACHABLE = 0;
			  }
			  else if (DESTINATION_UNREACHABLE){
				  printf("\n Destination unreachable, so cannot proceed further");
				  return 1;
			  }

			  count ++;
		  }
		  else if(written > 0 && proto == IPPROTO_ICMP)
		  {
			  //request timed out even with ICMP packets then stop after MAX_CONQ_ICMP_TIMEOUT hops
			  timed_out_cnt ++;
			  // If after consequtive MAX_CONQ_ICMP_TIMEOUT hops, request has timed out then stop
			  if(timed_out_cnt >= MAX_CONQ_ICMP_TIMEOUT) {
			    printf("\n Request has timed out even after %d hops, so not proceeding further\n",MAX_CONQ_ICMP_TIMEOUT);
			    return 1;
			  }
			  else {
			   printf("\n Request timed out");
               count ++;
			  }
			  
		  }
		  else if(written > 0 && FALLBACK_TO_ICMP)
		  {
              // Try again with ICMP if UDP has failed for the ttl
			  if(proto != IPPROTO_UDP)
				  count ++;
			  proto          = IPPROTO_ICMP; // Don't go with UDP hereafter
		  }
		  else if(FALLBACK_TO_ICMP == 0)
			  count ++;

	  }
  }
	  
  closesocket(sock);
  WSACleanup();
  return 0;
}
