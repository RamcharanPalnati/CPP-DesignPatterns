
#include <stdio.h>

int main(){
    char * x="HELLO WORLD";
    printf("%d\n",(int)sizeof(x));
    printf("%s\n",x);
    printf("%p\n",x);
    printf("%p\n",++x);
    printf("%c\n",*(x++));
    printf("%c\n",*x);
    printf("%s\n",x);
    printf("Length %d\n",(int)sizeof(x)/(int)sizeof(x[0]));
    printf("%d\n",(int)sizeof(x));
    printf("%d\n",(int)sizeof(&x));
    printf("%d\n",(int)sizeof(char));    
    return 0;
}