
#include <iostream>
#include <vector>
#include <string>

using namespace std;

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
    string command = "traceroute " + string(argv[1]) + " -n -q 1 -w 1-m 30 --icmp";
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
            
            networkInfo.hostIp = splitString[3];
            //networkInfo.hostIp = networkInfo.hostIp.substr(1, networkInfo.hostIp.size() - 2);
            networkInfo.latency = atof(splitString[5].c_str());
        }
        else
        {
            
            networkInfo.hostIp = splitString[2];
            //networkInfo.hostIp = networkInfo.hostIp.substr(1, networkInfo.hostIp.size() - 2);
            networkInfo.latency = atof(splitString[4].c_str());
        }

        traceInfo.push_back(networkInfo);
    }

    cout << "===============Packets are travelling through============" << endl;
    cout<< "hop\tIP Address\tLatency(ms)\n";

    for (auto iter2 = traceInfo.begin(); iter2 < (traceInfo.end()-1); ++iter2)
    {
        cout << (*iter2).hop <<  "\t" << (*iter2).hostIp << "\t" << (*iter2).latency << "ms" << endl;
    }

    return 0;
}
