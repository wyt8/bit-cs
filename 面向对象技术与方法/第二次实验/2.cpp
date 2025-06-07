#include<iostream>
using namespace std;

class CSmart
{ 
public:
    CSmart();
    ~CSmart();
private:
    // 指示是否调用过析构函数
    bool isCallDestroyFunction;
    static int numOfObj;
};

int CSmart::numOfObj = 0;

CSmart::CSmart()
{   
    numOfObj++;
    isCallDestroyFunction = false;
    cout << numOfObj << (numOfObj > 1 ? "objects" : "object") << endl;
}

CSmart::~CSmart()
{
    if(!isCallDestroyFunction)
    {
        numOfObj--;
        cout << numOfObj << (numOfObj > 1 ? "objects" : "object") << endl;
    }
    isCallDestroyFunction = true;
}
 
void DoSomething()
{
    CSmart s;
}
CSmart s1;


int main()
{
    // 结果如下：
    // 1object：s1创建
    // 2objects：s2创建
    // 3objects：DoSomething中的s创建
    // 2objects：DoSomething中的s摧毁
    // 3objects：new运算创建了一个匿名对象
    // 2objects：调用delet后释放了s3所指的对象
    // 1object：调用析构函数摧毁了s2
    // 0object：程序结束前摧毁全局变量s1
    CSmart s2;
    DoSomething();
    CSmart *s3 = new CSmart;
    delete s3;
    s2.~CSmart(); 
    return 0;
}
