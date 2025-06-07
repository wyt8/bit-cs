#include<iostream>
#include<string>
#include<ctime>
using namespace std;

char* week[7] = {"星期日", "星期一" , "星期二", "星期三", "星期四", "星期五", "星期六"};

class CDateTime
{
public:
    // 静态成员函数，返回包含当前时间日期的CDateTime对象
    static CDateTime Now();
    // tm 是 ctime 中定义的结构体，包含有时间和日期的相关信息
    CDateTime(struct tm dateTime);
    void ShowTime12();
    void ShowTime24();
    void ShowDate();
private:
    // 秒
    int sec;
    // 分
    int min;
    // 时
    int hour;
    // 一个月中的第几天，从1到31
    int mday;
    // 月份，从0到11
    int mon;
    // 自1900起的年数
    int year;
    // 一周中的第几天，从0到6
    int wday;
};

CDateTime CDateTime::Now()
{
    // 创建timt_t类型的变量
    time_t rawtime;
    // 返回自纪元起经过的时间，以秒为单位，存放在curtime中
    time(&rawtime);
    // 返回struct tm类型的时间，并用本地时区表示
    struct tm* curtime = localtime(&rawtime);
    return  CDateTime(*curtime);
}

CDateTime::CDateTime(struct tm dateTime)
{
    sec = dateTime.tm_sec;
    min = dateTime.tm_min;
    hour = dateTime.tm_hour;
    mday = dateTime.tm_mday;
    mon = dateTime.tm_mon;
    year = dateTime.tm_year;
    wday = dateTime.tm_wday;
}

void CDateTime::ShowTime12()
{
    cout << (hour >= 12 ? hour - 12 : hour) << ":" << min << ":" << sec 
        << (hour >= 12 ? " pm" : " am") << endl;
        
}

void CDateTime::ShowTime24()
{
    cout << hour << ":" << min << ":" << sec 
        << (hour >= 12 ? " pm" : " am") << endl;
}

void CDateTime::ShowDate()
{    
    cout << year + 1900 << "年" << mon + 1 << "月" << mday << "日，" << week[wday] << endl;
}



int main()
{
    CDateTime dt = CDateTime::Now();

    dt.ShowTime12(); // 以 am 或 pm 形式，显示当前时间，例如上午：10:11:12 am
    dt.ShowTime24(); // 以24小时形式，显示当前时间，例如下午：22:11:12 pm
    dt.ShowDate(); // 显示当前日期和星期，例如：2023年3月24日，星期五
    
    system("pause");
    return 0;
}