#pragma once
#include <iostream>
#include <string>
#include <fstream>

class Config
{
public:
	// 读取配置文件
	static void readConfigFile();

	static unsigned short UDPPort; // UDP端口
	static unsigned short DataSize; // PDU中数据字段的长度
	static int ErrorRate; // PDU错误率，如ErrorRate=10表示每10帧中一帧出错
	static int LostRate; // PUD丢失率，如LostRate=10，表示每10帧丢一帧
	static int SWSize; // 发送窗口大小
	static int InitSeqNo; // 起始PDU序号
	static int Timeout; // 超时定时器值，单位为毫秒

private:
	static std::string configFilePath; // 配置文件路径
};
