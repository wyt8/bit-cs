#include<iostream>
#include<string>
#include<ctime>
using namespace std;

char* week[7] = {"������", "����һ" , "���ڶ�", "������", "������", "������", "������"};

class CDateTime
{
public:
    // ��̬��Ա���������ذ�����ǰʱ�����ڵ�CDateTime����
    static CDateTime Now();
    // tm �� ctime �ж���Ľṹ�壬������ʱ������ڵ������Ϣ
    CDateTime(struct tm dateTime);
    void ShowTime12();
    void ShowTime24();
    void ShowDate();
private:
    // ��
    int sec;
    // ��
    int min;
    // ʱ
    int hour;
    // һ�����еĵڼ��죬��1��31
    int mday;
    // �·ݣ���0��11
    int mon;
    // ��1900�������
    int year;
    // һ���еĵڼ��죬��0��6
    int wday;
};

CDateTime CDateTime::Now()
{
    // ����timt_t���͵ı���
    time_t rawtime;
    // �����Լ�Ԫ�𾭹���ʱ�䣬����Ϊ��λ�������curtime��
    time(&rawtime);
    // ����struct tm���͵�ʱ�䣬���ñ���ʱ����ʾ
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
    cout << year + 1900 << "��" << mon + 1 << "��" << mday << "�գ�" << week[wday] << endl;
}



int main()
{
    CDateTime dt = CDateTime::Now();

    dt.ShowTime12(); // �� am �� pm ��ʽ����ʾ��ǰʱ�䣬�������磺10:11:12 am
    dt.ShowTime24(); // ��24Сʱ��ʽ����ʾ��ǰʱ�䣬�������磺22:11:12 pm
    dt.ShowDate(); // ��ʾ��ǰ���ں����ڣ����磺2023��3��24�գ�������
    
    system("pause");
    return 0;
}