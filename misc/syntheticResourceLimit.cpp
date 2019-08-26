#include <iostream>
#include <sys/resource.h>

#include <string>
using namespace std;
int main(){

    struct rlimit rlim;
    struct rlimit rlimAfterSet;

    int resources[]={RLIMIT_CORE,
                    RLIMIT_CPU,
                    RLIMIT_DATA,
                    RLIMIT_NOFILE,
                    RLIMIT_NPROC,
                    RLIMIT_RSS,
                    RLIMIT_STACK
                    };

    const char *names[]={"CORE",
                        "CPU",
                        "DATA",
                        "FSIZE",
                        "NPROC",
                        "RSS",
                        "STACK"};

    int n=sizeof(resources)/sizeof(resources[0]);
    int i;

    for(i=0;i<n;i++){
        getrlimit(resources[i],&rlim);
        cout<<"rlimit  : "<<names[i]<<" Soft : "<<rlim.rlim_cur<<" HardLimit: "<<rlim.rlim_max <<endl;
        
    }
    cout<<"After setti ng rlim "<<endl;
    rlim.rlim_cur=10000;
    //rlim.rlim_max=100000;

    for(i=0;i<n;i++){
        setrlimit(resources[i],&rlim);
        getrlimit(resources[i],&rlimAfterSet);
        cout<<"rlimit  : "<<names[i]<<" Soft : "<<rlimAfterSet.rlim_cur<<" HardLimit: "<<rlimAfterSet.rlim_max <<endl;
        
    }
    return 0;
    
}