#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <algorithm>


;

int main(int argc, char **argv)
{

    struct addrinfo *addrInfo,*p;
    struct sockaddr_in *hostAddress;

    string dnsHostName=argv[1];
    string nameServer="";
    
    
    if(argc>2){
        nameServer=argv[2];
        std::reverse(nameServer.begin(),nameServer.end());
        dnsHostName=nameServer+"."+dnsHostName;        

    }

    struct timeval startTime,endTime;
    memset(&startTime, 0, sizeof(startTime));
    memset(&endTime, 0, sizeof(endTime));

    gettimeofday(&startTime,NULL);
    int returnValue=getaddrinfo(dnsHostName.c_str(),NULL,NULL,&addrInfo);
    gettimeofday(&endTime,NULL);
    double resposneQueryTime= (double)(((1000000 * (endTime.tv_sec - startTime.tv_sec)) + (endTime.tv_usec - startTime.tv_usec)) / 1000.00);


    if(returnValue!=0){
        cout<<"ErrorInfo:"<<gai_strerror(returnValue)<<"  hostName:"<<dnsHostName<<endl;
    }else{
        for(p=addrInfo;p!=NULL;p=p->ai_next){

            hostAddress=(struct sockaddr_in*)p->ai_addr;

            if(p!=NULL){
                char chost[INET_ADDRSTRLEN];
                strcpy(chost,inet_ntoa(hostAddress->sin_addr));
                string resolvedIP=chost;
                cout<<"ResolvedIP: "<<resolvedIP<< " Responsetime: "<<resposneQueryTime<<endl;
            }

        }
        freeaddrinfo(addrInfo);
    }

    return 0;
}
