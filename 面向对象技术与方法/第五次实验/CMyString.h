#pragma once
#include<iostream>
#include<cstdlib>
#include<cmath>

const int increasementSpace = 20;

class CMyString
{
    friend CMyString& operator+ (const CMyString& left, const CMyString& right);
    friend std::ostream& operator<< (std::ostream& left, const CMyString& right);
    friend std::istream& operator>> (std::istream& left, CMyString& right);
public:
    CMyString()
    {
        m_length = 0;
        m_maxLength = 0;
        m_str = nullptr;
        reserveSpace(increasementSpace);
    }
    CMyString(const char* str);
    CMyString(const CMyString& str)
    {
        m_maxLength = str.m_maxLength;
        m_length = str.m_length;
        m_str = new char[m_maxLength];
        for(int i = 0; i < m_length; i++)
        {
            m_str[i] = str.m_str[i];
        }
    }
    CMyString(int num);
    CMyString& operator= (const char* str);
    CMyString& operator= (double num);
    char& operator[] (unsigned index);
    unsigned Find(char ch);
    unsigned Find(const char* str);
    CMyString Mid(unsigned start, unsigned len);
    operator int();
    // operator double();
    ~CMyString()
    {
        if(m_str)
            delete[] m_str;
    }
    int length()
    {
        return m_length;
    }
private:
    char* m_str;
    int m_length;
    // 当前空间所能保存字符串的最大长度
    int m_maxLength;
    // 预定空间
    bool reserveSpace(int expectedSpace);
    static int getStrLength(const char* str);
};

int CMyString::getStrLength(const char* str)
{ // 获取str的长度
    int i = 0;
    while(str[i] != '\0')
    {
        i++;
    }
    return i;
}

CMyString::CMyString(const char* str)
{
    m_length = 0;
    m_maxLength = 0;
    m_str = nullptr;
    int str_length = getStrLength(str);
    reserveSpace(str_length + increasementSpace);
    // 将str中的内容放在m_str中
    int i = 0;
    while(str[i] != '\0')
    {
        m_str[i] = str[i];
        i++;
    }
    m_length = i;
}

bool CMyString::reserveSpace(int expectedSpace)
{
    if(m_maxLength < expectedSpace)
    { // 当前的最大长度小于用户期望的长度就重新申请内存空间
        char* temp = new char[expectedSpace];
        if(temp == nullptr)
            return false;
        for(int i = 0; i < m_length; i++)
        {
            temp[i] = m_str[i];
        }
        if(m_str)
            delete[] m_str;
        m_str = temp;
        // 更新当前空间所能容纳字符串的最大长度
        m_maxLength = expectedSpace;
    }
    return true;
}

CMyString& CMyString::operator=(const char* str)
{
    int str_length = getStrLength(str);
    reserveSpace(str_length + increasementSpace);
    int i = 0;
    while(str[i] != '\0')
    {
        m_str[i] = str[i];
        i++;
    }
    m_length = i;
    return *this;
}

char& CMyString::operator[] (unsigned index)
{
    return m_str[index];
}

CMyString& operator+ (const CMyString& left, const CMyString& right)
{
    int leftLength = left.m_length;
    int rightLength = right.m_length;
    char* temp = new char[leftLength + rightLength + 5];

    int i = 0;
    for(int j = 0; j < left.m_length; j++)
    {
        temp[i] = left.m_str[j];
        i++;
    }
    for(int j = 0; j < right.m_length; j++)
    {
        temp[i] = right.m_str[j];
        i++;
    }
    temp[i] = '\0';
    CMyString*res = new CMyString(temp);
    delete[] temp;
    return *res;
}

std::ostream& operator<< (std::ostream& left, const CMyString& right)
{
    for(int i = 0; i < right.m_length; i++)
    {
        left << right.m_str[i];
    }
    return left;
}

std::istream& operator>> (std::istream& left, CMyString& right)
{
    char temp[4096];
    left >> temp;
    int length = right.getStrLength(temp);
    right.reserveSpace(length + increasementSpace);
    int i = 0;
    while(temp[i] != '\0')
    {
        right.m_str[i] = temp[i];
        i++;
    }
    right.m_length = length;
    return left;
}

unsigned CMyString::Find(char ch)
{
    for(int i = 0; i < m_length; i++)
    {
        if(m_str[i] == ch)
        {
            return i;
        }
    }
    return -1;
}

unsigned CMyString::Find(const char* str)
{
    for(int i = 0; i < m_length; i++)
    {
        int j = 0;
        for(j = 0; str[j] != '\0'; j++)
        {
            if(!(m_str[i + j] == str[j]))
            {
                break;
            }
        }
        if(str[j] == '\0')
        {
            return i;
        }
    }
    return -1;
}

CMyString CMyString::Mid(unsigned start, unsigned len)
{
    char temp[100];
    int i = 0;
    for(i = 0; i < len; i++)
    {
        temp[i] = m_str[start + i];
    }
    temp[i] = '\0';
    return CMyString(temp);
}

CMyString::CMyString(int num)
{
    m_length = 0;
    m_maxLength = 0;
    m_str = nullptr;
    char temp[100];
    sprintf(temp, "%d", num);
    reserveSpace(100);
    int i = 0;
    for(i = 0; temp[i] != '\0'; i++)
    {
        m_str[i] = temp[i];
    }
    m_length = i;
}

CMyString::operator int ()
{
    return atoi(m_str);
}

CMyString& CMyString::operator= (double num)
{
    char temp[100];
    sprintf(temp, "%f", num);
    reserveSpace(100);
    int len = getStrLength(temp);
    for(int i = 0; i < len; i++)
    {
        m_str[i] = temp[i];
    }
    m_length = len;
    return *this;
}

// CMyString::operator double()
// {
//     return strtod(m_str, nullptr);
// }
