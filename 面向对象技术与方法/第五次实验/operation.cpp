#include<iostream>
#include"CMyString.h"
using namespace std;

class CSqlStatement
{
public:
    CSqlStatement(CMyString str):sql(str) 
    {// 将sql中的问号替换为数字1/2/3...
        int index = 1;
        int pos = sql.Find('?');
        while(pos != -1)
        {
            CMyString front = sql.Mid(0, pos);
            CMyString back = sql.Mid(pos + 1, sql.length() - pos - 1);
            sql = front + CMyString(index) + back;
            index++;
            pos = sql.Find('?');
        }
    }
    void SetAttribute(CMyString index, CMyString str);
    void ExecuteSql();
private:
    CMyString sql;
};

void CSqlStatement::SetAttribute(CMyString index, CMyString str)
{// 找到相应的位置，将其一分为二，进行字符串相加
    if(str.Find("||") != -1 || str.Find("&&") != -1)
    {
        throw "Errors in setting attribution";
    }
    int pos = sql.Find((int)index + '0');
    CMyString front = sql.Mid(0, pos);
    CMyString back = sql.Mid(pos + 1, sql.length() - pos - 1);
    sql = front + str + back;
}

void CSqlStatement::ExecuteSql()
{
    cout << "sql: " << sql << endl; 
}



int main()
{
    CSqlStatement sql ="select ?, ? from student where SID = ?";
    sql.SetAttribute("1", "Name");
    sql.SetAttribute("2", "Age");
    // sql.SetAttribute("3", "2020007"); // 或者：sql.SetAttribute("3", "abc || 2023 == 2023"); 这时，成员函数应抛出异常：Errors in setting attribution
    try
    {
        sql.SetAttribute("3", "abc || 2023 == 2023");
    }
    catch(const char* error) // 捕获异常以显示信息
    {
        cout << error << endl;
    }
    sql.ExecuteSql( );
    return 0;
}