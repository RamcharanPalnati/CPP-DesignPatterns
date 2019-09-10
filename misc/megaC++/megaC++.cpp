#include <iostream>
#include <string>
#define COUT std::cout
#define ENDL std::endl

void beforeMain();
void afterMain();

#pragma startup beforeMain  // gcc compiler wont work here
#pragma exit afterMain

/*For gcc compiler to work
void __attribute__((constructor)) beforeMain(); 
void __attribute__((destructor)) afterMain(); 
*/

//#pragma startup func1 -- functions which we need to run before control goes to main function
//#pragma exit func2 -- functions which we need to run after control comes from  main function

void whatHappensWhenWeCrossRangeOfDataType();
void crossLimitOfShort();


int main()
{

    whatHappensWhenWeCrossRangeOfDataType();
    crossLimitOfShort();
    return 0;
}

void crossLimitOfShort()
{
    COUT << "//remove this if you want to run" << ENDL;
    return;
    short a;
    //a=75000;//warning: overflow in implicit constant conversion [-Woverflow]
    COUT<<a<<ENDL;
    
    for (a = 32767; a < 32770; a++)//infinite loop since char ranges from -32767 to  32767 and it wont cross  32770
        COUT << a << "\n";

    return;
}
void whatHappensWhenWeCrossRangeOfDataType()
{
    auto count = 0;
    COUT << "//remove this if you want to run" << ENDL;
    return;
    for (char i = 220; i < 250; ++i, ++count)
    { //infinite loop since char ranges from -127 to 128 and it wont cross 255
        std::cout << count << "\t" << i << "\t";
    }
    std::cout << std::endl;
}
void beforeMain(){
    COUT<<"Before Main"<<ENDL;
}
void afterMain(){
    COUT<<"After Main"<<ENDL;
}