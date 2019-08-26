#include <iostream>
#include <string>

template <typename T>
class templateX
{

    T x;
    T *y;

public:
    templateX();
    ~templateX();
    void printx();
    void setx(std::string x);
    void setx(T x);
    
};

template <typename T>
templateX<T>::templateX()
{
    std::cout << "created dynamica memory T" << std::endl;
    y = new T;
}
template <typename T>
templateX<T>::~templateX()
{
    std::cout << "deleted dynamic memory created T" << std::endl;
    delete y;
}
template <typename T>
void templateX<T>::setx(T x)
{
    this->x = x;
}


template <typename T>
void templateX<T>::setx(std::string x)
{
    std::cout<<"called explicit string"<<std::endl;
    this->x = x;
}


template <typename T>
void templateX<T>::printx()
{
    std::cout << this->x << std::endl;
}

int main()
{
    templateX<int> intx;
    templateX<std::string> stringy;
    intx.setx(23);
    intx.printx();
    stringy.setx("1234");
    stringy.printx();
    return 0;
}
