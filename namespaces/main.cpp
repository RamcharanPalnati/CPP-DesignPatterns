#include "namespace.h"


int main(){
    
    nameSpace::sampleClass * sample=new nameSpace::sampleClass();
    nameSpace1::sampleClass * sample1=new nameSpace1::sampleClass();

    sample->printClass();
    sample1->printClass();

    delete sample;
    delete sample1;

    return 0;    
}