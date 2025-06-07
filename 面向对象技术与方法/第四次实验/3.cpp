#include<iostream>
using namespace std;

const double PI = 3.1415926;


class CShape
{ // 抽象类
public:
    virtual double Area() = 0;
    virtual double Volume() = 0;
};


class CSphere: public CShape
{ // 实现基类中的虚函数
public:
    // 要在派生类的初始化列表中对基类进行初始化
    CSphere(double r): CShape(), radius(r) {}
    double Area()
    {
        return 4 * PI * radius * radius;
    }
    double Volume()
    {
        return 4 * PI * radius * radius * radius / 3;
    }
private:
    double radius;
};


class CCylinder: public CShape
{
public:
    CCylinder(double r, double h): CShape(), radius(r), height(h) {}
    double Area()
    {
        return 2 * PI * radius * radius + height * PI * radius * 2;
    }
    double Volume()
    {
        return height * PI * radius * radius;
    }
private:
    // 圆柱底部圆的半径
    double radius;
    double height;
};


class CAD
{ // 通过虚函数体现了多态的特性
private:
    CShape& shape;
public:
    CAD(CShape& S):shape(S) {}
    double Area( ) { return shape.Area(); }
    double Volume( ) { return shape.Volume(); }
};


int main()
{
    CSphere sphere(4.5);
    CAD cad1(sphere);
    cout << "球的体积：" << cad1.Volume() << " 球的表面积：" << cad1.Area() << endl;
    CCylinder cylinder(4.5, 10.1);
    CAD cad2(cylinder);
    cout << "圆柱的体积：" << cad2.Volume() << " 圆柱的表面积：" << cad2.Area() << endl;
    system("pause");
    return 0;
}