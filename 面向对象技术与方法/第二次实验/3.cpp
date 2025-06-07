#include<iostream>
#include<bitset>
using namespace std;

class CIntChar
{
public:
    CIntChar(char s[]);
    void ASC_Print();
    void Binary_Print();
    void Int_Print();
private:
    int content;    
};

CIntChar::CIntChar(char s[])
{
    int i = 0;
    // 将 int 类型的 content 的内存地址强制转换为 char 类型的地址
    void* temp = static_cast<void*>(&content);
    char* charPtr = static_cast<char*>(temp);
    // 将内容由内存高位到低位存储，不足的在低位补0
    while(s[i] != '\0' && i < 4)
    {
        charPtr[3 - i] = s[i];
        i++;
    }
    while(i < 4)
    {
        charPtr[3 - i] = 0;
        i++;
    }
}

void CIntChar::ASC_Print()
{
    int i = 3;
    void* temp = static_cast<void*>(&content);
    char* charPtr = static_cast<char*>(temp);
    // 从高位到低位读取每字节
    while(charPtr[i] != 0 && i >= 0)
    {
        cout << charPtr[i];
        i--;
    }
    cout << endl;
}

void CIntChar::Binary_Print()
{
    // 以二进制形式表示content
    cout << bitset<32>(content) << endl;
}

void CIntChar::Int_Print()
{
    cout << content << endl;
}

int main()
{
    CIntChar IC("Lov");

    IC.ASC_Print();
    IC.Binary_Print();
    IC.Int_Print();
    
    system("pause");
    return 0;
}