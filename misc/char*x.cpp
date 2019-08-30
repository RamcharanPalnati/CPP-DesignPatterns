#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;
int main(){
    char * s="343";   //
    cout<<"const string literal is a address"<<endl;
    printf(" value of  s   s p:%p\n",s);
    s=s+2;
    printf(" address of  s   &s:%p\n",&s);
    printf(" value of  s   s p:%p\n",s);
    printf(" string of  s   %% s :%s\n",s);
    s="dwewqfqrgrefgrevea";

    cout<<"fterchanget43905"<<endl;

    printf(" address of  s   &s:%p\n",&s);
    printf(" value of  s   s p:%p\n",s);
    printf(" string of  s   %% s :%s\n",s);


    char s2[2]="31";
    cout<<"s2 cout "<<s2<<endl;
    printf(" address of  s2 &s:%p\n",&s2);
    printf(" value of  s   s2 p:%p\n",s2)                                                                                                                                                                                                                                                                                                                                                                                                                            ;
    printf(" string of  s    s2 :%s\n",s2);
    printf(" string of  s  s2[2] :%c\n",s2[2]);
    printf(" string of  s  s2[3] :%c\n",s2[3]);
    cout<<"fterchanget43905"<<endl;

    s2[2]='7';
    s2[3]='8';
    printf(" address of  s2 &s:%p\n",&s2);
    printf(" value of  s   s2 p:%p\n",s2)                                                                                                                                                                                                                                                                                                                                                                                                                            ;
    printf(" string of  s    s2 :%s\n",s2);
    printf(" string of  s    s2[2] :%c\n",s2[2]);
    printf(" string of  s    s2[3] :%c\n",s2[3]);

    char *s3= new char[10];
    char *s4=s3;
    for(int i=0;i<10;i++){
        *s4='a';
        ++s4;

    }
    printf("s3:%s\n",s3);
    cout<<s3<<endl;
    delete s3;



    s3= new char[7];
    cout<<"s3"<<s3<<endl;
    char *s5=s3;
    for(int i=0;i<3;i++){
        *s5='b';
        ++s5;

    }
    printf("s3:%s\n",s3);
    cout<<s3<<endl;
    delete s3;

    char *z=(char *)malloc(sizeof(char)*5);
    cout<<"z"<<z<<endl;
    free(z);
    return 0;
}