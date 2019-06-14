#include <iostream>

using namespace std;

class Singleton
{

    static Singleton *SingletonInstance;    
    int data;
    Singleton()
    {
        data=5;
    }

public:
    static Singleton *getInstance()
    {
        if (SingletonInstance == NULL)
        {
            SingletonInstance = new Singleton();
        }
        return SingletonInstance;
    }
    void setData(int x)
    {
        data = x;
    }
    int getData()
    {
        return data;
    }
};

Singleton *Singleton::SingletonInstance = NULL;
int main()
{
    cout<<"Instance:"<<Singleton::getInstance()->getData()<<endl;
    Singleton *Instance = Singleton::getInstance();
    Instance->setData(35);
    cout << "Instance1:" << Instance->getData() << endl;
    Singleton *Instance2 = Singleton::getInstance();
    cout << "Instance2:" << Instance2->getData() << endl;
    Singleton *Instance3 = Singleton::getInstance();
    Instance3->setData(39);
    cout << "Instance3:" << Instance3->getData() << endl;
    cout << "Instance2:" << Instance2->getData() << endl;
    cout << "Instance1:" << Instance->getData() << endl;
}