#include <cstdio>


static int returnStatic(){
    return 5;
}
struct staticInt{
    staticInt(){
        
    }
};
int main(){
   static int i = 7;
   i=returnStatic();
   i=i*i;
    printf("%d\n",i);
    return 0;
}