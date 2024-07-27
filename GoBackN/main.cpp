#include <string>
#include <iostream>

#include "ThreadSafeQueue.h"
#include "global.h"
#include "NetworkLayer.h"
#include "DataLinkLayer.h"
#include "PhysicalLayer.h"
#include "Config.h"
#include "Logger.h"

using namespace std;

int main() {
    Config::readConfigFile();
    ThreadSafeQueue<EventType> eventQueue;
    PhysicalLayer physicalLayer(&eventQueue, Config::UDPPort, Config::ErrorRate, Config::LostRate);
    bool isServer = physicalLayer.isServer();
    if (isServer) {
        Logger::createLogFile("server");
        cout << "我是服务端" << endl;
    }
    else {
        Logger::createLogFile("client");
        cout << "我是客户端" << endl;
    }
    DataLinkLayer dataLinkLayer(&eventQueue, Config::InitSeqNo, Config::SWSize, Config::Timeout, &physicalLayer);
    NetworkLayer networkLayer(&eventQueue, &dataLinkLayer);
    
    cout << "1、添加要发送的文件" << endl;
    cout << "2、退出程序" << endl;
    int select;
    bool flag = true;
    while (flag) {
        cout << endl << "请输入您的选项：" << endl;
        if (!(std::cin >> select)) {
            std::cin.clear(); // 清除错误状态  
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); // 忽略输入流中的剩余字符  
            std::cout << "输入错误，请输入一个整数选项！" << std::endl;
            continue; // 重新开始循环  
        }
        switch (select) {
        case 1:
        {
            cout << "请输入文件路径：";
            string filePath;
            cin >> filePath;
            networkLayer.addFilePath(filePath);
            break;
        }
        case 2:
            flag = false;
            cout << "再见！" << endl;
            break;
        default:
            cout << "输入的选项不合法！" << endl;
        }
    }

    Logger::closeLogFile();
    return 0;
}