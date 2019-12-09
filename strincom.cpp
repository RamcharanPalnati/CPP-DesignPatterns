#include <iostream>
#include <string>
using namespace std;

int main(){
    string x="reqrereqwqt=qeqrwrew";
    x=x.substr(x.find("=")+1);
    cout<<x<<endl;
    
}