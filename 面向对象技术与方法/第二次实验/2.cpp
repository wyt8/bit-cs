#include<iostream>
using namespace std;

class CSmart
{ 
public:
    CSmart();
    ~CSmart();
private:
    // ָʾ�Ƿ���ù���������
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
    // ������£�
    // 1object��s1����
    // 2objects��s2����
    // 3objects��DoSomething�е�s����
    // 2objects��DoSomething�е�s�ݻ�
    // 3objects��new���㴴����һ����������
    // 2objects������delet���ͷ���s3��ָ�Ķ���
    // 1object���������������ݻ���s2
    // 0object���������ǰ�ݻ�ȫ�ֱ���s1
    CSmart s2;
    DoSomething();
    CSmart *s3 = new CSmart;
    delete s3;
    s2.~CSmart(); 
    return 0;
}
