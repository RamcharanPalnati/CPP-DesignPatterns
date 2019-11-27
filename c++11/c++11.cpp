#include <iostream>
#include <thread>
#define cout std::cout
#define cin std::cin
#define endl "\n"
#define string std::string

void executeDefaultUsage();
void executeDeleteUsage();
void executeLambda();
void executeNullptrUsage();
void executeRvalueRefUsage();
void executeCharUsage();

class baseClass
{
    int x;

public:
    baseClass() = default; // no need to implement
    baseClass(const baseClass &)=delete;
    baseClass(int x)
    {
        this->x = x;
        cout << "Setx in Base class\n";
    }
    virtual ~baseClass() = default;
    baseClass & operator =(const baseClass&)=delete;    
};
int main()
{
    int choice = -1;

    cout << "###### C++11,14,17 Tutorials ####" << endl;

    for (;;)
    {
        cout << "\n\nEnter Your Choice:\n";
        cout << "1.Default implementation of constructor and 2.destructor\n";
        cout << "2.Delete implementation of constructor and 2.Desctructor\n";
        cout << "3.Lambda Expressions"<<endl;
        cout << "4.Usage of nullptr over NULL"<<endl;
        cout << "5.Rvalue references \n";
        cout <<"6.Char* usage\n";
        cout <<"Or Press AnyButton to Exit "<<endl;

        cin >> choice;

        switch (choice)
        {
        case 1:
            executeDefaultUsage();
            break;
        case 2:
            executeDeleteUsage();
            break;
        case 3:
            executeLambda();
            break;
        case 4:
            executeNullptrUsage();
            break;
        case 5: 
            executeRvalueRefUsage();
            break;
        case 6:
            executeCharUsage();
            break;
        default:
            cout << "####Leaving the tutorial ####\n";
            return 0;
        }
    }
}

void executeDefaultUsage()
{
    cout << "1.Default implementation of constructor and 2.destructor\n";
    baseClass *base1 = new baseClass(5);
    baseClass *base2 = new baseClass();
    delete base1, base2;
}

void executeDeleteUsage()
{
    cout << "2.Delete implementation of constructor and 2.Desctructor\n";
    
    //  ##UNCOMMENT BELOW FOR UNDERSTANDING


    /*baseClass base1(3);
    baseClass base2=base1;
    baseClass base3;
    base3=base1;
    */
    
}


void executeLambda()
{
    cout << "3.Lambda Expressions"<<endl<<endl;
    cout <<"Lambdas are inline,faster than normal function call\n";
    cout<<"We have to Implement when the code is not reusable  & not necessary\n";
    
    cout<<"[] example\ "<<endl;
    auto z =[](int x,int y ){if(x>y){return x;}else {return y;}};
    cout<<"z:"<<z(2,3)<<endl;
    cout<<"z:"<<z(5,3)<<endl;

    auto x=[](){cout<<"Lambda Without return type printing\n";};
    x();

}


void executeNullptrUsage()
{
    cout << "4.Usage of nullptr over NULL"<<endl;
    //We can assign NULL to anything 
    //int x=NULL;
    int *y=NULL;
    //cout<<"NULL Value :"<<NULL<<endl;

    //But we cannot assign nullptr to non-npointers

    //  ##UNCOMMENT BELOW FOR UNDERSTANDING
    //int z=nullptr;
    int *u =nullptr;
    //cout<<"nullptr value:"<<nullptr<<endl;
}


void executeRvalueRefUsage(){
    cout<<"Under Construction"<<endl;
}
void executeCharUsage()
{
    char *x = "1235";    
    char y[10]="12345";
    static int rt=0;
    for(int i =0;y[i]!='\0';++i){
        cout<<y[i]<<"\t";
        ++rt;
    }
    cout<<"count:"<<rt<<"\n";
    cout<<"thread hardware concurrency:"<<std::thread::hardware_concurrency();
}