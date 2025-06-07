#include<iostream>
#include<cmath>
using namespace std;


class CDate
{
public:
    CDate(int y, int m, int d);
    int Span(const CDate& cdate) const;
    bool isLeapYear() const;
    int getTotalDays() const;    
private:
    
    
    int Year, Month, Day;
};

CDate::CDate(int y, int m, int d):Year(y), Month(m), Day(d) {}

int CDate::getTotalDays() const
//获取当前日期距离1900年1月1日的天数
{
    int totalDays = 0;
    for(int i = Year - 1; i >= 1900; i--)
    {   
        CDate temp(i, 1, 1);
        if(temp.isLeapYear())
            totalDays += 366;
        else
            totalDays +=365;
    }
    for(int i = 1; i < Month; i++)
    {
        switch(i)
        {
            case 1: case 5: case 7: case 8: case 10: case 12:
                totalDays += 31;
                break;
            case 2:
                if(isLeapYear())
                    totalDays += 29;
                else
                    totalDays +=28;
                break;
            default:
                totalDays += 30;
                break;
        }
    }
    totalDays += Day;
    return totalDays;
}

bool CDate::isLeapYear() const
//判断是否为闰年
{
    if(Year % 4 == 0 && Year % 100 !=0 || Year % 400 ==0)
        return true;
    return false;
}

int CDate::Span(const CDate& cdate) const
{
    int totalDays1 = getTotalDays();
    int totalDays2 = cdate.getTotalDays();
    // 间隔天数要加1
    return abs(totalDays1 - totalDays2) + 1;

}


int main()
{
    CDate date1(2023, 2, 27), date2(2023, 3, 10);
    int spanDays = date2.Span(date1);
    cout << spanDays << endl;
    system("pause");
    return 0;
}