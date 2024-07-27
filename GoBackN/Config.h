#pragma once
#include <iostream>
#include <string>
#include <fstream>

class Config
{
public:
	// ��ȡ�����ļ�
	static void readConfigFile();

	static unsigned short UDPPort; // UDP�˿�
	static unsigned short DataSize; // PDU�������ֶεĳ���
	static int ErrorRate; // PDU�����ʣ���ErrorRate=10��ʾÿ10֡��һ֡����
	static int LostRate; // PUD��ʧ�ʣ���LostRate=10����ʾÿ10֡��һ֡
	static int SWSize; // ���ʹ��ڴ�С
	static int InitSeqNo; // ��ʼPDU���
	static int Timeout; // ��ʱ��ʱ��ֵ����λΪ����

private:
	static std::string configFilePath; // �����ļ�·��
};
