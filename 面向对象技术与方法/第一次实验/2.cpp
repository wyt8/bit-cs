#include<iostream>
using namespace std;


void PressButton(void (*fp)());
void Show();
void Popmenu();


int main()
{
    // 通过函数指针传参来在函数内部调用函数
    PressButton(Show);
    PressButton(Popmenu);
    system("pause");
    return 0;
}


void PressButton(void (*fp)())
// fp是一个函数指针
{
    fp();
}


void Show()
{
    cout << "Call Show function" << endl;
}


void Popmenu()
{
    cout << "Call Popmenu function" << endl;
}