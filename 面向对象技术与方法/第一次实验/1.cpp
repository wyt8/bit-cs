#include<iostream>
#include<vector>
#include<algorithm>
#include<fstream>
#include<string>
using namespace std;


// 统计中文字符总数目
int getWordsNum(string filename);
// 统计名字在文章中出现的次数
int appearTimes(string filename, string name);


int main()
{
    string filename = "E:\\C++\\JourneyToWest.txt";
    cout << getWordsNum(filename) << endl;

    // 要统计名字出现次数的名单
    vector<string> nameList{ "大圣", "玉帝", "王母娘娘", "仙女", "天王" };
    // 根据名字出现的次数进行排序
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
    // 将文件读指针定位到开头
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
                // 计算汉字码点，lowCode表示低字节的值，highCode表示高字节的值
                int lowCode = (line[i + 1] + 256) % 256;
                int highCode = (line[i] + 256) % 256;
                //highCode小于127， 表明是按1字节储存的，不是汉字
                if (highCode <= 127)
                {
                    i++;
                    continue;
                }

                int characterCode = highCode * 256 + lowCode;
                // 0xB0A1~0xF7FE是国标2312中汉字的码点范围
                
                if (characterCode >= 0xB0A1 && characterCode <= 0xF7FE)
                {
                    wordNum++;
                }
                // 一个汉字两字节
                i += 2;
            }

        }
    }
    else
        cout << "文件打开失败" << endl;
    fin.close();
    return wordNum;
}


int appearTimes(string filename, string name)
{
    ifstream fin(filename);
    //将文件读指针定位到开头
    fin.seekg(0, ios::beg);
    int times = 0;
    if (!fin)
        cout << "文件打开失败" << endl;
    string line;
    // 统计子串出现的次数
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
