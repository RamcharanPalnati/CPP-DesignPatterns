#include <iostream>
#include <string>

inline std::string max(std::string t1,std::string t2){
    return t1+t2;
}

template <typename T>
inline T max(T t1, T t2)
{
    return t1 > t2 ? t1 : t2;
}
template <typename T1,typename T2>
inline T1 max(T1 t1, T2 t2)
{
    return t1+t2;
}


int main(){
    std::string c1="ram";
    std::string c2="ram";
    std::cout<<max(1,2)<<std::endl;
    std::cout<<max(1.4,2.4)<<std::endl;
    std::cout<<max('c','d')<<std::endl;
    std::cout<<max("cd","ce")<<std::endl;
    std::cout<<max(c1,c2)<<std::endl;
    std::cout<<max(1.4,2)<<std::endl;
    std::cout<<max(1,2.5)<<std::endl;
    return 0;
}
