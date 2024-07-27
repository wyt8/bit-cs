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
        cout << "���Ƿ����" << endl;
    }
    else {
        Logger::createLogFile("client");
        cout << "���ǿͻ���" << endl;
    }
    DataLinkLayer dataLinkLayer(&eventQueue, Config::InitSeqNo, Config::SWSize, Config::Timeout, &physicalLayer);
    NetworkLayer networkLayer(&eventQueue, &dataLinkLayer);
    
    cout << "1�����Ҫ���͵��ļ�" << endl;
    cout << "2���˳�����" << endl;
    int select;
    bool flag = true;
    while (flag) {
        cout << endl << "����������ѡ�" << endl;
        if (!(std::cin >> select)) {
            std::cin.clear(); // �������״̬  
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); // �����������е�ʣ���ַ�  
            std::cout << "�������������һ������ѡ�" << std::endl;
            continue; // ���¿�ʼѭ��  
        }
        switch (select) {
        case 1:
        {
            cout << "�������ļ�·����";
            string filePath;
            cin >> filePath;
            networkLayer.addFilePath(filePath);
            break;
        }
        case 2:
            flag = false;
            cout << "�ټ���" << endl;
            break;
        default:
            cout << "�����ѡ��Ϸ���" << endl;
        }
    }

    Logger::closeLogFile();
    return 0;
}