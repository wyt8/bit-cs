#include<iostream>
#include "CMyString.h"
using namespace std;


int main()
{
    CMyString s1("BIT"), s2;
    s2 = "love";
    s2[0] = 'L';
    cout << "I " + s2 << " " << s1 << endl; // The output is: I Love BIT
    int pos = s1.Find('T'); // pos is 2, or -1 if 'T' is not found.
    cout << pos << endl;
    pos = s1.Find("IT"); // pos is 1, or -1 if "IT" is not found.
    cout << pos << endl;
    pos = s1.Find("b"); // pos is -1
    cout << pos << endl;
    int startPos = 1, len = 2;
    // startPos represents starting position, len represents the length of substring. So Mid can 
    // get a substring: ov.
    CMyString my = s2.Mid(startPos, len);
    cout << my << endl;
    CMyString str_num(123);
    cout << str_num << endl;
    int a = str_num;
    cout << a << endl;
    str_num = -123.456;
    cout << str_num << endl;
    double b = str_num;
    cout << b << endl;
    system("pause");
    return 0;
}