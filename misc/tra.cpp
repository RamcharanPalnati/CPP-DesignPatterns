#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/un.h>

#include <poll.h>
#include <fcntl.h>


#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#include <signal.h>

#include <time.h>
#include<sys/time.h>
#include <pthread.h>

#define SOCKET_ERROR 0
using namespace std;
// Variables for tracerouting
int main()
{
    int sockRaw;
    struct sockaddr_in dest, from;
    int ret, datasize, done = 0, maxhops = 30, ttl = 1;
    socklen_t fromlen = sizeof(from);
    char icmpData[1024];
    char recvBuffer[1024];
    char destIP[16] = "172.26.96.150";
    unsigned short seq_no = 0;

    struct timeval timeout;

    int hopTimeoutCount = 0;

    // Create a raw socket
    sockRaw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockRaw == SOCKET_ERROR)
    {
        printf("Failed to create tracert socket, errno: %d\n", errno);
        pthread_exit((void *)1);
        return 1;
    }
    // Set timeout values
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    ret = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
    if (ret == SOCKET_ERROR)
    {
        printf("Failed to set receive timeout, errno: %d\n", errno);
        pthread_exit((void *)1);
        return 1;
    }
    ret = setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval));
    if (sockRaw == SOCKET_ERROR)
    {
        printf("Failed to set send timeout, errno: %d\n", errno);
        pthread_exit((void *)1);
        return 1;
    }
    memset(&dest, 0, sizeof(struct sockaddr_in));
    dest.sin_family = AF_INET;
    // Set the destination address
    if ((dest.sin_addr.s_addr = inet_addr(destIP)) == INADDR_NONE)
    {
        printf("Failed to set destination ip, errno: %d\n", errno);
        pthread_exit((void *)1);
        return 1;
    }
    memset(icmpData, 0, sizeof(icmpData));
    datasize = sizeof(icmpData);
    datasize += sizeof(IcmpHeader);
    // Fill in ICMP header data
    IcmpHeader *icmp_hdr;
    char *datapart;
    icmp_hdr = (IcmpHeader *)icmpData;
    icmp_hdr->i_type = ICMP_ECHO;
    icmp_hdr->i_code = 0;
    icmp_hdr->i_id = (unsigned short)getpid();
    icmp_hdr->i_cksum = 0;
    icmp_hdr->i_seq = 0;
    datapart = icmpData + sizeof(IcmpHeader);
    memset(datapart, 'E', datasize - sizeof(IcmpHeader));

    for (ttl = 1; ((ttl < maxhops) && (!done)); ttl++)
    {
        // Set the time to live
        ret = setsockopt(sockRaw, IPPROTO_IP, IP_TTL, (int *)&ttl, sizeof(int));
        if (ret == SOCKET_ERROR)
        {
            printf("Failed to set TTL, errno: %d\n", errno);
            pthread_exit((void *)1);
            return ((void *)1);
        }
        ((IcmpHeader *)icmpData)->i_seq = seq_no++;
        ((IcmpHeader *)icmpData)->i_cksum = checksum((unsigned short *)icmpData, datasize);
        // Send the ICMP packet
        ret = sendto(sockRaw, icmpData, datasize, 0, (struct sockaddr *)&dest, sizeof(dest));
        if (ret == SOCKET_ERROR)
        {
            printf("Failed to send ICMP, errno: %d\n", errno);
            pthread_exit((void *)1);
            return ((void *)1);
        }
        // Read the packet
        ret = recvfrom(sockRaw, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr *)&from, &fromlen);
        if (ret == SOCKET_ERROR)
        {
            printf("%2d error: %d\n", ttl, errno);
            // Proceed to the next hop
            hopTimeoutCount++;
            if (hopTimeoutCount == 5)
            {
                // Too many timeouts, the destination is probably not responding to ICMP packets
                printf("Traceroute finished (no response)\n");
                pthread_exit((void *)1);
                return ((void *)1);
            }
            continue;
        }
        // Decode the response
        IpHeader *iphdr = NULL;
        IcmpHeader *icmphdr = NULL;
        unsigned short iphdrlen;
        struct hostent *lpHostent = NULL;
        struct in_addr inaddr = from.sin_addr;
        iphdr = (IpHeader *)recvBuffer;
        iphdrlen = iphdr->h_len * 4;
        if (ret < iphdrlen + ICMP_MIN)
            printf("Too short response from %s\n", inet_ntoa(from.sin_addr));
        icmphdr = (IcmpHeader *)(recvBuffer + iphdrlen);
        char host[NI_MAXHOST];
        struct sockaddr_in address;
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr = inaddr;
        switch (icmphdr->i_type)
        {
        case ICMP_ECHOREPLY:
            lpHostent = gethostbyaddr((const char *)&from.sin_addr, 4, AF_INET);
            done = 1;
            break;
        case ICMP_TIMEOUT:
            if (getnameinfo((struct sockaddr *)&address, sizeof(address), host, sizeof(host), NULL, 0, NI_NAMEREQD) == 0)
            {
                printf("%2d %s %s\n", ttl, inet_ntoa(inaddr), host);
            }
            else
            {
                printf("%2d %s\n", ttl, inet_ntoa(inaddr));
            }
            hopTimeoutCount = 0;
            done = 0;
            break;
        case ICMP_DESTUNREACH:
            printf("%2d %s unreachable\n", ttl, inet_ntoa(inaddr));
            done = 1;
            break;
        default:
            printf("Non-echo type %d received\n", icmphdr->i_type);
            done = 1;
            break;
        }
    }
    printf("Traceroute finished\n");
    pthread_exit((void *)1);
    return 1;
}