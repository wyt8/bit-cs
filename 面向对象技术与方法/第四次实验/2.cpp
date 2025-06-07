#include<iostream>
#include "CMyString.h"
using namespace std;
const int maxLength = 100;

class CInternetURL
{
public:
    CInternetURL(): url()
    {
        res = new char[maxLength];
    }
    CInternetURL(const char* str): url(str)
    {
        res = new char[maxLength];
    }
    char* GetDomain();
    char* GetDomainCountry();
    char* GetDomainType();
    char* GetHomePage();
    ~CInternetURL()
    {
        delete[] res;
    }
private:
    CMyString url;
    // �ó�Ա���ڱ������ַ���ָ��
    char* res;
    int getLength(const char* str)
    { // ��ȡ�ַ����ĳ���
        int i = 0;
        while(str[i] != '\0')
        {
            i++;
        }
        return i;
    }
};


char* CInternetURL::GetDomain()
{ // ��á�://���͡�/��֮����ַ��Ӵ���Ϊ����
    int start = url.Find("://");
    int j = 0;
    if(start != -1)
    {
        for(int i = start + 3; url[i] != '/'; i++)
        {
            res[j++] = url[i];
        }
        res[j++] = '\0';
    }
    return res;
}

char* CInternetURL::GetDomainCountry()
{ // ��domain��ȡ���һ����֮�������
    char* domain = GetDomain();
    int j = 0;
    for(int i = getLength(domain) - 1; domain[i] != '.'; i--)
    {
        res[j++] = domain[i];
    }
    res[j] = '\0';
    j--;
    // ���ղŵõ����ַ������з�ת�͵õ��˹���
    for(int i = 0; i < j; i++)
    {
        char temp = res[i];
        res[i] = res[j];
        res[j] = temp;
    }
    return res;
}

char* CInternetURL::GetDomainType()
{ // ��domain�ĵ����ڶ���͵�����һ����֮������ݽ�ȡ�͵õ�����վ����
    char* domain = GetDomain();
    int len = getLength(domain);
    int pos = len - 1;
    while(domain[pos] != '.')
    {
        pos--;
    }
    int j = 0;
    for(int i = pos - 1; domain[i] != '.'; i--)
    {
        res[j++] = domain[i];
    }
    res[j] = '\0';
    j--;
    for(int i = 0; i < j; i++, j--)
    {
        char temp = res[i];
        res[i] = res[j];
        res[j] = temp;
    }
    return res;
}

char* CInternetURL::GetHomePage()
{ // ��ȡdomain�ġ�/����������ݾ�����ҳ
    char* domain = GetDomain();
    int j = 0;
    for(int i = url.Find(domain) + getLength(domain) + 1; url[i] != '\0'; i++)
    {
        res[j++] = url[i];
    }
    res[j] = '\0';
    return res;
}


int main()
{
    CInternetURL URL("https://jwc.bit.edu.cn/index.htm"), s2;
    cout << URL.GetDomain() << endl; // The result is: www.bit.edu.cn
    cout << URL.GetDomainCountry() << endl; // The result is: cn
    cout << URL.GetDomainType() << endl; // The result is: edu
    cout << URL.GetHomePage() << endl; // The result is: index.htm
    system("pause");
    return 0;
}