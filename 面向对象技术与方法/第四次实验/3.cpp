#include<iostream>
using namespace std;

const double PI = 3.1415926;


class CShape
{ // ������
public:
    virtual double Area() = 0;
    virtual double Volume() = 0;
};


class CSphere: public CShape
{ // ʵ�ֻ����е��麯��
public:
    // Ҫ��������ĳ�ʼ���б��жԻ�����г�ʼ��
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
    // Բ���ײ�Բ�İ뾶
    double radius;
    double height;
};


class CAD
{ // ͨ���麯�������˶�̬������
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
    cout << "��������" << cad1.Volume() << " ��ı������" << cad1.Area() << endl;
    CCylinder cylinder(4.5, 10.1);
    CAD cad2(cylinder);
    cout << "Բ���������" << cad2.Volume() << " Բ���ı������" << cad2.Area() << endl;
    system("pause");
    return 0;
}