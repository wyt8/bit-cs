#include<iostream>
#include<algorithm>
using namespace std;

const int numOfDigits = 110;

class CLINT
{
private:
    char bigNum[numOfDigits];
    int digits;

public:
    CLINT Add(const CLINT& L); // Achieve the sum of two big numbers
    void Value(); // Display the big number
    CLINT(string str);
    CLINT();
};

CLINT::CLINT()
{
    digits = 0;
    // C����ַ����Ľ�β�� '\0'
    bigNum[0] = '\0';
}

CLINT::CLINT(string str)
{
    digits = str.size();
    copy(str.begin(), str.end(), bigNum);
    // C����ַ����Ľ�β�� '\0'
    bigNum[digits] = '\0';
}

void CLINT::Value()
{
    cout << bigNum << endl;
}

CLINT CLINT::Add(const CLINT& L)
{
    string res;
    int leftIndex = this->digits - 1;
    int rightIndex = L.digits - 1;
    // flag �ǽ�λ��־
    int flag = 0;
    while(leftIndex >= 0 && rightIndex >= 0)
    {
        int ch = (bigNum[leftIndex] + L.bigNum[rightIndex] - 2 * '0' + flag) % 10 + '0';
        flag = (bigNum[leftIndex] + L.bigNum[rightIndex] - 2 * '0' + flag) / 10;
        res.push_back(ch);
        leftIndex--, rightIndex--;
    }
    // �����һ���ַ���������ϣ�ֱ�Ӵ�����һ���ַ���
    if(leftIndex == -1)
    {
        while(rightIndex >= 0)
        {
            int ch = (L.bigNum[rightIndex] - '0' + flag) % 10 + '0';
            flag = (L.bigNum[rightIndex] - '0' + flag) / 10;
            res.push_back(ch);
            rightIndex--;
        }
    }
    else
    {
        while(leftIndex >= 0)
        {
            int ch = (bigNum[leftIndex] - '0' + flag) % 10 + '0';
            flag = (bigNum[leftIndex] - '0' + flag) / 10;
            res.push_back(ch);
            leftIndex--;
        }
    }
    // ��ת����ַ���
    reverse(res.begin(), res.end());
    return CLINT(res);
}

int main()
{
    CLINT L1("12345678900987654321"), L2("9876543210"), L3;
    L3 = L1.Add(L2);
    L3.Value(); // 12345678910864197531
    system("pause");
    return 0;
}