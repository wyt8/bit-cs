#include<iostream>
#include<vector>
#include<algorithm>
#include<fstream>
#include<string>
using namespace std;


// ͳ�������ַ�����Ŀ
int getWordsNum(string filename);
// ͳ�������������г��ֵĴ���
int appearTimes(string filename, string name);


int main()
{
    string filename = "E:\\C++\\JourneyToWest.txt";
    cout << getWordsNum(filename) << endl;

    // Ҫͳ�����ֳ��ִ���������
    vector<string> nameList{ "��ʥ", "���", "��ĸ����", "��Ů", "����" };
    // �������ֳ��ֵĴ�����������
    sort(nameList.begin(), nameList.end(),
        [filename](string a, string b) {return appearTimes(filename, a) > appearTimes(filename, b); });

    cout << "Two MVP are:" << endl;
    cout << nameList[0] << ", who appears " << appearTimes(filename, nameList[0])
        << " times." << endl;
    cout << nameList[1] << ", who appears " << appearTimes(filename, nameList[1])
        << " times." << endl;

    system("pause");
    return 0;
}


int getWordsNum(string filename)
{
    ifstream fin(filename);
    int wordNum = 0;
    // ���ļ���ָ�붨λ����ͷ
    fin.seekg(0, ios::beg);
    string line;
    if (fin)
    {
        while (getline(fin, line))
        {
            for (int i = 0; i < line.size();)
            {
                if (i == line.size() - 1)
                    break;
                // ���㺺����㣬lowCode��ʾ���ֽڵ�ֵ��highCode��ʾ���ֽڵ�ֵ
                int lowCode = (line[i + 1] + 256) % 256;
                int highCode = (line[i] + 256) % 256;
                //highCodeС��127�� �����ǰ�1�ֽڴ���ģ����Ǻ���
                if (highCode <= 127)
                {
                    i++;
                    continue;
                }

                int characterCode = highCode * 256 + lowCode;
                // 0xB0A1~0xF7FE�ǹ���2312�к��ֵ���㷶Χ
                
                if (characterCode >= 0xB0A1 && characterCode <= 0xF7FE)
                {
                    wordNum++;
                }
                // һ���������ֽ�
                i += 2;
            }

        }
    }
    else
        cout << "�ļ���ʧ��" << endl;
    fin.close();
    return wordNum;
}


int appearTimes(string filename, string name)
{
    ifstream fin(filename);
    //���ļ���ָ�붨λ����ͷ
    fin.seekg(0, ios::beg);
    int times = 0;
    if (!fin)
        cout << "�ļ���ʧ��" << endl;
    string line;
    // ͳ���Ӵ����ֵĴ���
    while (getline(fin, line))
    {
        int i = 0;
        while (line.find(name, i) != -1)
        {
            times++;
            i = line.find(name, i) + 1;
        }
    }
    fin.close();
    return times;
}
