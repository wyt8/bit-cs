#include<iostream>
#include<string>
#include<stack>
#include "CMyStack.h"
using namespace std;

class CExpression
{
private:
    string expression;
    // 判断是否为运算符
    bool isopt(char ch);
    // 获取运算符的优先级
    int getPriority(char ch);
    // 二元表达式的计算
    double calc(double a, double b, char op);
    double str2double(string str)
    {// 将字符串型的小数转换为double
        return stod(str, 0);
    }
public:
    double Value( ); // Get value of expresstion
    friend ostream& operator<< (ostream& os, const CExpression& expr); // print only expression except its value
    CExpression(string str):expression(str) {};
    void SetExpression(string str);
};

double CExpression::calc(double a, double b, char op)
{
    if(op == '+')
        return a + b;
    if(op == '-')
        return a - b;
    if(op == '*')
        return a * b;
    if(op == '/')
        return a / b;
}

int CExpression::getPriority(char ch)
{
    if(ch == '+' || ch == '-')
        return 1;
    if(ch == '*' || ch == '/')
        return 2;
    if(ch == '(')
        return 0;
    if(ch == ')')
        return 3; 
}

bool CExpression::isopt(char ch)
{
    if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')')
        return true;
    
    return false;
}

void CExpression::SetExpression(string str)
{
    expression = str;
}

double CExpression::Value()
{
    CMyStack nums(20);
    stack<char> op;
    string doubleNum;
    for(int i = 0; i < expression.size(); i++)
    {
        if(isopt(expression[i]))
        {
            // 将上次的数字转化为double并存入栈中
            if(doubleNum.size())
                nums.Push(str2double(doubleNum));
            doubleNum = "";

            // 如果是‘(’就直接存入符号栈中
            if(expression[i] == '(')
            {
                op.push(expression[i]);
                continue;
            }

            // 如果是‘)’则计算之前的表达式，并消去‘(’
            if(expression[i] == ')')
            {
                while(op.top() != '(')
                {
                    double b = nums.Pop();
                    double a = nums.Pop();
                    double res = calc(a, b, op.top());
                    nums.Push(res);
                    op.pop();
                }
                if(op.top() == '(')
                {
                    op.pop();
                    continue;
                }
            }
            // 其他情况判断优先级，并进行计算
            while(!op.empty() && getPriority(op.top()) >= getPriority(expression[i]))
            {
                if(nums.GetSize() >= 2)
                {// 当操作数的个数大于2时才能进行计算
                    double b = nums.Pop();
                    double a = nums.Pop();
                    double res = calc(a, b, op.top());
                    nums.Push(res);
                    op.pop(); 
                }
                else
                    break;
            }
            if(!(op.empty() && expression[i] == ')'))
                // 不能再符号栈空时push进‘)’
                op.push(expression[i]);
        }

        else
        {
            doubleNum.push_back(expression[i]);
        }
    }

    // 进行后续处理
    if(doubleNum.size())
        nums.Push(str2double(doubleNum));
    while(!op.empty())
    {
        if(nums.GetSize() >= 2)
        {
            double b = nums.Pop();
            double a = nums.Pop();
            double res = calc(a, b, op.top());
            nums.Push(res);
            op.pop(); 
        }
    }
    return nums.Peek();
}

ostream& operator<< (ostream& os, const CExpression& expr)
{// 友元函数重载运算符
    os << expr.expression;
    return os;
}

int main()
{
    CExpression expr("50.3-20.12+8*8/2");
    cout << expr << " = " << expr.Value() << endl; // 50.3-20.12+8*8/2 = 62.18
    expr.SetExpression("55.99-88.11+77.12");
    cout << expr << " = " << expr.Value() << endl; // 55.99-88.11+77.12 = 45
    expr.SetExpression("(39+11)*30+10/5");
    cout << expr << " = " << expr.Value() << endl; // (39+11)*30+10/5 = 1502
    expr.SetExpression("39+12*(47+33)");
    cout << expr << " = " << expr.Value() << endl; // 39+12*(47+33) = 999
    expr.SetExpression("20/(112-(10*1.2))/10-1.01");
    cout << expr << " = " << expr.Value() << endl; // 20/(112-(10*1.2))/10-1.01 = -0.99
    expr.SetExpression("20/(112-10*1.2)/10-1.01");
    cout << expr << " = " << expr.Value() << endl; // 20/(112-10*1.2)/10-1.01 = -0.99
    cout << "ENDING..." << endl;
    system("pause");
    return 0;
}
