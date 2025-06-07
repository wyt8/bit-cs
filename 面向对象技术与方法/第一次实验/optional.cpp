#include<iostream>
using namespace std;


bool resetMemory(const void* startPtr, int num, int initValue)
{
    // 当初值不合理或数目小于1时，输入不合法
    if(initValue < 0 || initValue >255 || num <= 0)
        return false;
    const char* tempPtr = static_cast<const char*>(startPtr);
    char* charPtr = const_cast<char*>(tempPtr);
    for(int i = 0; i < num; i++)
    {
        *(charPtr + i) = initValue;
    }
    return true;
}

int main()
{
    const int arrayLength = 20;
    int test[arrayLength];
    for(int i = 0; i < arrayLength; i++)
    {
        cout << test[i] << "*" ;
    }
    cout << endl;
    cout << "After resetMemory" << endl;
    resetMemory(test, 15 * sizeof(int), 0);
    for(int i = 0; i < arrayLength; i++)
    {
        cout << test[i] << "*" ;
    }
    cout << endl;
    system("pause");
    return 0;
}