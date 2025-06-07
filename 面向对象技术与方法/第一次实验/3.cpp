#include<iostream>
#include<cmath>
using namespace std;


class CCircle
{
public:
    CCircle(double x = 0, double y = 0, double r = 0);
    void Relateion(const CCircle& ccircle) const;
private:
    double X, Y;
    double radius;
};

CCircle::CCircle(double x, double y, double r):X(x), Y(y), radius(r) {}

void CCircle::Relateion(const CCircle& ccircle) const
{
    double anotherX = ccircle.X;
    double anotherY = ccircle.Y;
    double anotherRadius = ccircle.radius;
    double distance = sqrt((X - anotherX) * (X - anotherX) + (Y - anotherY) * (Y - anotherY));
    //通过圆心之间的距离和两圆的半径和来判断两元的位置关系
    if (distance > radius + anotherRadius)
        cout << "seperation" << endl;
    else
        cout << "intersection" << endl;
}


int main()
{
    CCircle c1(10, 2, 4), c2(2, 3, 1);
    c1.Relateion(c2);
    system("pause");
    return 0;
}