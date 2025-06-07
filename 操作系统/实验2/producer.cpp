#include <iostream>
#include "BufferPoolManager.h"
#include <cstdlib>
#include <ctime>
#include <string>

std::string getRandomString(int length); // 生成随机字符串

int main(int argc, char *argv[])
{
    std::cout << "Start Producer ID = " << argv[0] << std::endl;
    BufferPoolManager bpm;
    srand(time(nullptr));
    for (int i = 0; i < 12; i++)
    {
        Sleep(rand() / 10);
        std::string str = getRandomString(10);
        bpm.write(str, "Producer " + std::string(argv[0]));
    }
    return 0;
}

std::string getRandomString(int maxLength)
{
    int flag, i;
    srand(time(nullptr));
    int length = rand() % maxLength + 1;
    std::string res(length, '*');
    for (i = 0; i < length; i++)
    {
        flag = rand() % 3;
        switch (flag)
        {
        case 0:
            res[i] = 'A' + rand() % 26;
            break;
        case 1:
            res[i] = 'a' + rand() % 26;
            break;
        case 2:
            res[i] = '0' + rand() % 10;
            break;
        default:
            res[i] = '*';
            break;
        }
    }
    return res;
}