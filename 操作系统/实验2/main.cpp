#include<windows.h>
#include<iostream>

int main() {
    STARTUPINFO producerSi[2]; // 记录结构体有多大，必须要参数
	PROCESS_INFORMATION producerPi[2];    // 进程id，进程句柄，线程id，线程句柄存在于这个结构体
    STARTUPINFO consumerSi[3]; // 记录结构体有多大，必须要参数
	PROCESS_INFORMATION consumerPi[3];    // 进程id，进程句柄，线程id，线程句柄存在于这个结构体

    for(int i = 0; i < 2; i++) {
        ZeroMemory(&producerSi[i], sizeof(producerSi[i]));
        producerSi[i].cb = sizeof(producerSi[i]);
        ZeroMemory(&producerPi[i], sizeof(producerPi[i]));
        char argv[2] = {'0'+i,'\0'};
        CreateProcess("producer.exe", argv, NULL, NULL, FALSE, 0, NULL, NULL, &producerSi[i], &producerPi[i]);
    }

    for(int i = 0; i < 3; i++) {
        ZeroMemory(&consumerSi[i], sizeof(consumerSi[i]));
        consumerSi[i].cb = sizeof(consumerSi[i]);
        ZeroMemory(&consumerPi[i], sizeof(consumerPi[i]));
        char argv[2] = {'0'+i,'\0'};
        CreateProcess("consumer.exe", argv, NULL, NULL, FALSE, 0, NULL, NULL, &consumerSi[i], &consumerPi[i]);
    }
    Sleep(200000);
    std::cout << "Main finished!" << std::endl;
    system("pause");
    return 0;
}
