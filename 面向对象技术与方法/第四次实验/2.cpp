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
    // 该成员用于保存结果字符串指针
    char* res;
    int getLength(const char* str)
    { // 获取字符串的长度
        int i = 0;
        while(str[i] != '\0')
        {
            i++;
        }
        return i;
    }
};


char* CInternetURL::GetDomain()
{ // 获得“://”和“/”之间的字符子串即为域名
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
{ // 对domain截取最后一个点之后的内容
    char* domain = GetDomain();
    int j = 0;
    for(int i = getLength(domain) - 1; domain[i] != '.'; i--)
    {
        res[j++] = domain[i];
    }
    res[j] = '\0';
    j--;
    // 将刚才得到的字符串进行翻转就得到了国家
    for(int i = 0; i < j; i++)
    {
        char temp = res[i];
        res[i] = res[j];
        res[j] = temp;
    }
    return res;
}

char* CInternetURL::GetDomainType()
{ // 将domain的倒数第二点和倒数第一个点之间的内容截取就得到了网站类型
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
{ // 截取domain的“/”后面的内容就是首页
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