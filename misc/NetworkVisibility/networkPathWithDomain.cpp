
#include <iostream>
#include <vector>
#include <string>
#include "httplib.h"
#include <curl/curl.h>
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output

using namespace std;

size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size * nmemb;
    try
    {
        s->append((char *)contents, newLength);
    }
    catch (std::bad_alloc &e)
    {
        //handle memory problem
        return 0;
    }
    return newLength;
}

string httpClientExample(string IP)
{
    try
    {
        CURL *curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_DEFAULT);

        curl = curl_easy_init();
        std::string s;
        if (curl)
        {
            string url = "https://freegeoip.live/json/" + IP; //IP or domain
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //only for https
            //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //only for https
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
            //curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L); //remove this to disable verbose output

            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if (res != CURLE_OK)
            {
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
            }

            /* always cleanup */
            curl_easy_cleanup(curl);
        }

        return s;
    }
    catch (exception &e)
    {
        std::cout << "Exception Occcured in httpClientExample" << e.what() << std::endl;
    }
}
struct NetworkPath
{
    int hop;
    float latency;
    string hostIp;
    string domainName;
};
size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos)
    {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    string info = txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1);
    strs.push_back(info);

    return strs.size();
}

int main(int argc, char *argv[])
{

    string command = "traceroute " + string(argv[1]) + " -q 1 -w 1-m 30 --icmp";
    FILE *process = popen(command.c_str(), "r");
    char buff[1024];
    int hopCounter = 0;
    vector<string> info;
    if (process != NULL)
    {
        while (!feof(process))
        {

            fgets(buff, sizeof(buff), process);
            if (hopCounter == 0)
            {
                ++hopCounter;
                continue;
            }
            //cout<<"buffer:"<<buff<<endl;

            info.push_back(string(buff));
        }
    }

    vector<NetworkPath> traceInfo;
    int hopCount = 0;
    int hopCounter1 = 0;

    for (auto iter = info.begin(); iter != info.end(); iter++)
    {
        //cout <<hopCounter1<<""<<*iter;
        vector<string> splitString;
        ++hopCounter1;
        if ((*iter).find("*") != std::string::npos)
        {
            continue;
        }
        split(*iter, splitString, ' ');

        //cout << endl;
        ++hopCount;
        NetworkPath networkInfo;
        networkInfo.hop = hopCount;

        if (hopCounter1 < 10)
        {
            networkInfo.domainName = splitString[3];
            networkInfo.hostIp = splitString[4];
            networkInfo.hostIp = networkInfo.hostIp.substr(1, networkInfo.hostIp.size() - 2);
            networkInfo.latency = atof(splitString[6].c_str());
        }
        else
        {
            networkInfo.domainName = splitString[2];
            networkInfo.hostIp = splitString[3];
            networkInfo.hostIp = networkInfo.hostIp.substr(1, networkInfo.hostIp.size() - 2);
            networkInfo.latency = atof(splitString[5].c_str());
        }

        traceInfo.push_back(networkInfo);
    }

    cout << "===============Packets are travelling through============" << endl;
    

    for (auto iter2 = traceInfo.begin(); iter2 < (traceInfo.end() - 1); ++iter2)
    {
        cout <<"\n\n=====HOP Number ====:"<<(*iter2).hop<<endl;
        cout << "DomainName :"<<(*iter2).domainName << "\n"<<"Latency" << (*iter2).latency << "ms" << endl;
        string jsonParsedInfo = httpClientExample((*iter2).hostIp);
        rapidjson::Document document;
        document.Parse<0>(jsonParsedInfo.c_str());
        if (document["ip"].GetString() != "")
        {
            std::cout <<"IP :"<<document["ip"].GetString() << "\t";
        }
        else
        {
            std::cout <<"IP :"<< "NA"
                      << "\t";
        }
        cout<<endl;
        if (document["country_code"].GetString() != "")
        {
            std::cout <<"country_code :"<< document["country_code"].GetString() << "\t";
        }
        else
        {
            std::cout<<"country_code :" << "NA"
                      << "\t";
        }
        cout<<endl;
        if (document["country_name"].GetString() != "")
        {
            std::cout<<"country_name :" << document["country_name"].GetString() << "\t";
        }
        else
        {
            std::cout<<"country_name :" << "NA"
                      << "\t";
        }
        cout<<endl;
        if (document["region_name"].GetString() != "")
        {
            std::cout<<"region_name :" << document["region_name"].GetString() << "\t";
        }
        else
        {
            std::cout<<"region_name :" << "NA"
                      << "\t";
        }
        cout<<endl;
        if (document["city"].GetString() != "")
        {
            std::cout <<"city :"<< document["city"].GetString() << "\t";
        }
        else
        {
            std::cout <<"city :"<< "NA"
                      << "\t";
        }
        cout<<endl;
        if (document["time_zone"].GetString() != "")
        {
            std::cout <<"time_zone :"<< document["time_zone"].GetString() << "\t";
        }        
        else
        {
            std::cout <<"time_zone :"<< "NA"
         
                      << "\t";
        }
        cout<<endl;
        

        /*"ip" : "172.25.81.134",
            "country_code" : "", "country_name" : "", "region_code" : "", "region_name" : "", "city" : "", "zip_code" : "", "time_zone" : "", 
            "latitude" : 0, "longitude" : 0, "metro_code" : 0
            */
    }


    return 0;
}
