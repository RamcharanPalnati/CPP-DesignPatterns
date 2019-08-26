//When you need subclass based on some data ,you have to use factory design pattern.
//Factory manufactures object of  a class based on choice

#include <iostream>

using namespace std;

class Car
{
public:
    virtual void printCarName() = 0;
    virtual ~Car() = 0;
    Car()
    {
        cout << "Lets construct a Car" << endl;
    }
};

Car::~Car()
{

    cout << "Destructing the car" << endl;
}

class RolceRoyce : public Car
{
public:
    void printCarName()
    {
        cout << "This is RolceRoyce car bro ....chill !!" << endl;
    }
    ~RolceRoyce()
    {
        cout << "Bro you destroyed the RolceRoyce CARR !!!" << endl;
    }
};

class Tata : public Car
{
public:
    void printCarName()
    {
        cout << "This is Tata car bro.India's Volvo. !!" << endl;
    }
    ~Tata()
    {
        cout << "I know its very harder to destroy TATA car !!!" << endl;
    }
};

class FordMustang : public Car
{
public:
    void printCarName()
    {
        cout << "This is 500 Horses FordMustang car bro" << endl;
    }
    ~FordMustang()
    {
        cout << "Bro you destroyed the FordMustang CARR !!!" << endl;
    }
};

Car *getYourCarBasedOnChoice(int choice)
{
    switch (choice)
    {
    case 1:
        return new RolceRoyce;
        break;
    case 2:
        return new Tata;
        break;
    case 3:
        return new FordMustang;
        break;
    default:
        return new Tata;
        break;
    }
}

int main()
{
    int choice;
    Car *car;
    for (int i = 0; i < 4; ++i)
    {
        cout << "\nEnter Choice Of Car :" << endl;
        cin >> choice;
        car = getYourCarBasedOnChoice(choice);
        car->printCarName();
        delete car;
    }
    return 0;
}