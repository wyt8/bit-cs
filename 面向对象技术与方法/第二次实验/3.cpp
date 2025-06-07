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
    // �� int ���͵� content ���ڴ��ַǿ��ת��Ϊ char ���͵ĵ�ַ
    void* temp = static_cast<void*>(&content);
    char* charPtr = static_cast<char*>(temp);
    // ���������ڴ��λ����λ�洢��������ڵ�λ��0
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
    // �Ӹ�λ����λ��ȡÿ�ֽ�
    while(charPtr[i] != 0 && i >= 0)
    {
        cout << charPtr[i];
        i--;
    }
    cout << endl;
}

void CIntChar::Binary_Print()
{
    // �Զ�������ʽ��ʾcontent
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