#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>

using namespace std;

int main(){

string command,nameserver,queryResult;
char buff[1024];
cout<<"Enter host:"<<endl;
cin>>command;
cout<<"Enter nameserver:"<<endl;
cin>>nameserver;
FILE * process;
command="nslookup "+command+" "+nameserver;
cout<<command<<endl;
process = popen(command.c_str(), "r");
    if (process != NULL) {
        while (!feof(process)) {
            fgets(buff, sizeof(buff), process);
            queryResult += buff;
        }
        pclose(process);
    }
    cout<<command<<endl;
    cout<<"Response:\n"<<queryResult<<endl;

return 0;
}