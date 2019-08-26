#include <iostream>
#include <hiredis.h>
#include <ctime>
#include <string>
#include <cstring>
#include<unistd.h>
#include <string>
#include <sstream>
using namespace std;

int main()
{
    int seconds=std::time(0);
    std::stringstream ss;
    ss << seconds;
    string uuid=ss.str();
    cout << "UUID" << uuid << endl;
    redisContext *redisClient;
    redisReply *tempReply;
    struct timeval redisTimeout = {30, 0}; //redisTimeout
    redisClient = redisConnectWithTimeout("0.0.0.0", 8102, redisTimeout);
    cout<<"step1"<<endl;
    int syncTime=0;  
    tempReply = (redisReply *)redisCommand(redisClient, "GET %s", "configStartFlag");
    cout<<"step2"<<endl;
    if (tempReply->str != NULL)
    {
        cout<<"configStartFlag:"<<tempReply->str;
        freeReplyObject(tempReply);
        cout<<"step3"<<endl;
        tempReply =(redisReply *)redisCommand(redisClient, "TTL %s", "heartBeat");
        cout<<"step4"<<endl;
        int timeToWait=tempReply->integer;

        freeReplyObject(tempReply);
        cout<<"step5"<<endl;
        sleep(timeToWait+10);
        while(1){
            cout<<"step6"<<endl;
            tempReply = (redisReply *)redisCommand(redisClient, "LPUSH %s %s", "heartBeat",uuid.c_str());
            
            sleep(300);
        }

    }
    else
    {
        cout<<"step7"<<endl;
        freeReplyObject(tempReply);
        cout<<"step8"<<endl;
        tempReply = (redisReply *)redisCommand(redisClient, "SET %s %s", "configStartFlag",uuid.c_str());
        cout<<"step9"<<endl;
            freeReplyObject(tempReply);
            cout<<"step10"<<endl;
    while(1){
        cout<<"step11"<<endl;
        tempReply = (redisReply *)redisCommand(redisClient, "LPUSH %s %s", "heartBeat",uuid.c_str());
        if(tempReply->str!=NULL){
            cout<<tempReply->str<<endl;
        }
        freeReplyObject(tempReply);
        cout<<"step12"<<endl;
        tempReply =(redisReply *)redisCommand(redisClient, "EXPIRE %s %s", "heartBeat", "300");
        if(tempReply->str!=NULL){
            cout<<tempReply->str<<endl;
        }
        freeReplyObject(tempReply);
        cout<<"step13"<<endl;
        tempReply =(redisReply *)redisCommand(redisClient, "TTL %s", "heartBeat");
        if(tempReply->str!=NULL){
            cout<<tempReply->str<<endl;
        }
        int timeWait=tempReply->integer;
        cout<<"timeWeight:"<<timeWait<<endl;
        freeReplyObject(tempReply);
        cout<<"step14"<<endl;
        sleep(timeWait+1);
    }


    }

    return 0;
}