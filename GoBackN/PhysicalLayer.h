#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <unordered_set>
#include <string>
#include <thread>
#include <random>

#include "Frame.h"
#include "ThreadSafeQueue.h"
#include "global.h"

//���� ws2_32.dll
#pragma comment(lib, "ws2_32.lib")

/*
* �����
* ʵ�ʲ���UDP Socket������ģ�Ⲣʵ��Frame�ķ��ͺͽ��գ�ÿ��UDP���ݱ���װһ��Frame
* ��������Ͳ���Ҫ���з�֡�Ȳ�����Frame��Ҳ���ð���֡����ʼ��־�ͽ�����־
*/
class PhysicalLayer
{
public:
	PhysicalLayer(ThreadSafeQueue<EventType>* eventQueue, unsigned short serverPort, int errorRate, int lostRate)
		: eventQueue(eventQueue), errorRate(errorRate), lostRate(lostRate) {
		bool isServer = false;
		// ͨ�������ź������жϸó����Ƿ��Ϊ�����
		HANDLE mutexHandle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"gobackn");
		if (mutexHandle == NULL) {
			mutexHandle = CreateMutex(NULL, FALSE, L"gobackn");
			this->server = true;
		}
		else {
			this->server = false;
		}

		// ���÷���˵�ַ�ṹ
		memset(&this->serverAddr, 0, sizeof(this->serverAddr));
		this->serverAddr.sin_family = AF_INET;
		this->serverAddr.sin_port = serverPort;
		inet_pton(this->serverAddr.sin_family, "127.0.0.1", &this->serverAddr.sin_addr.s_addr);

		// ���г�ʼ��
		this->inited = this->init();

		// �����߳�ģ��������շ���Ϣ
		if (this->inited) {
			std::thread t(&PhysicalLayer::run, this);
			t.detach();
		}
	}

	~PhysicalLayer() {
		HANDLE mutexHandle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"gobackn");
		if (this->server && mutexHandle != NULL) {
			ReleaseMutex(mutexHandle);
		}
		if (this->inited) {
			// �ر��׽���
			closesocket(this->sockfd);
			WSACleanup();
		}
	}

	// �Ƿ�Ϊ�����
	bool isServer() const {
		return this->server;
	}
	
	// ����������Ҫ���͵�֡
	void send(Frame frame) {
		this->sendFrameQueue.push(frame);
	}

	// ��ȡ�������յ���֡
	bool receive(Frame& frame) {
		return this->receiveFrameQueue.try_pop(frame);
	}

private:
	bool init() {
		// ��ʼ��Winsock
		if (WSAStartup(MAKEWORD(2, 2), &this->wsaData) != 0) {
			std::cerr << "WSAStartup failed" << std::endl;
			return false;
		}

		// �����׽���
		this->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sockfd == INVALID_SOCKET) {
			std::cerr << "Socket creation failed" << std::endl;
			WSACleanup();
			return false;
		}

		u_long iMODE = 1; // 1Ϊ��������0Ϊ����
		ioctlsocket(this->sockfd, FIONBIO, &iMODE); // ����recvfrom�Ƿ�Ϊ����

		// �������Ҫ�󶨶˿ڣ����ͻ��˲���Ҫ�󶨶˿ڣ�����Ҫ�������������Ϣ�÷�������ס�ͻ���
		if (this->server) {
			// ���׽���
			if (bind(this->sockfd, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr)) != 0) {
				std::cerr << "Bind failed" << std::endl;
				closesocket(this->sockfd);
				WSACleanup();
				return false;
			}
		}
		else {
			char message[] = "client";
			sendto(this->sockfd, message, sizeof(message), 0, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr));
		}

		return true;
	}

	bool server; // �Ƿ�Ϊ�����
	bool inited; // �Ƿ�����˳�ʼ��
	int errorRate; // ������
	int lostRate; // ��ʧ��
	SOCKET sockfd;
	WSADATA wsaData;
	sockaddr_in serverAddr;
	ThreadSafeQueue<EventType>* eventQueue; // �¼�����

	ThreadSafeQueue<Frame> sendFrameQueue; // Ҫ���͵�֡����
	ThreadSafeQueue<Frame> receiveFrameQueue; // ���յ���֡����

	std::unordered_set<unsigned short> clientPorts; // �ͻ��˶˿ڼ��ϣ����ڷ����ʹ��

	void run() {
		bool isReceiveEmpty = false;
		while (true) {
			//if (sendFrameQueue.size() == 0 && isReceiveEmpty) {
			//	std::this_thread::sleep_for(std::chrono::milliseconds(100)); // ���߳�����100ms������ϵͳ����
			//}

			// ��Ҫ���͵�֡������ȡ��֡����
			Frame frame;
			if (this->sendFrameQueue.try_pop(frame)) {
				if (frame.type == FrameType::data) {
					// ģ�ⶪʧ
					if (this->lostRate > 0) {
						// ��ʼ�������������
						std::random_device rd;  // ���ڻ������
						std::mt19937 gen(rd()); // ʹ������豸���ӳ�ʼ��Mersenne Twister������
						std::uniform_int_distribution<> distrib(1, this->lostRate); // ����ֲ���Χ

						// ��ȡһ�������
						int randomNumber = distrib(gen);

						if (randomNumber == 1) {
							std::cout << "[physical] ֡��ʧ" << std::endl;
							goto sendFinish;
						}
					}
				}
				

				int len = frame.getLength();
				char* buffer = new char[len];
				memset(buffer, 0, len);
				frame.getByteArray(buffer, &len);
				
				// ģ�����
				if (this->errorRate > 0 && frame.type==FrameType::data) {
					// ��ʼ�������������
					std::random_device rd;  // ���ڻ������
					std::mt19937 gen(rd()); // ʹ������豸���ӳ�ʼ��Mersenne Twister������
					std::uniform_int_distribution<> distrib(1, this->errorRate); // ����ֲ���Χ

					// ��ȡһ�������
					int randomNumber = distrib(gen);

					if (randomNumber == 1) {
						buffer[2] = 'w';
						std::cout << "[physical] ֡����" << std::endl;
					}
				}

				if (this->server) {
					// ����������пͻ��˷��͸�֡
					for (auto& clientPort : this->clientPorts) {
						sockaddr_in clientAddr;
						memset(&clientAddr, 0, sizeof(clientAddr));
						clientAddr.sin_family = AF_INET;
						clientAddr.sin_port = clientPort;
						inet_pton(clientAddr.sin_family, "127.0.0.1", &clientAddr.sin_addr.s_addr);
						sendto(this->sockfd, buffer, len, 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
						//std::cout << "[physical] ����֡�����˿ڣ�" << clientPort << std::endl;
					}
				}
				else {
					// �ͻ���һ���������������Ϣ
					sendto(this->sockfd, buffer, len, 0, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr));
					//std::cout << "[physical] ����֡�����˿ڣ�" << (unsigned short)this->serverAddr.sin_port << std::endl;
				}
				delete[] buffer;
			}
			//else {
			//	std::this_thread::sleep_for(std::chrono::milliseconds(20)); // ���߳�����20ms������ϵͳ����
			//}

		sendFinish:

			// ����֡�洢������֡������
			int len = MAX_FRAME_LENGTH;
			char* buffer = new char[len];
			memset(buffer, 0, len);
			sockaddr_in sockaddr;
			int socklen = sizeof(sockaddr);
			memset(&sockaddr, 0, sizeof(sockaddr));
			if (recvfrom(this->sockfd, buffer, len, 0, (struct sockaddr*)&sockaddr, &socklen) > 0) {
				if (this->server && strcmp("client", buffer) == 0) {
					std::cout << "[info] �����µĿͻ��ˣ��˿�Ϊ��" << sockaddr.sin_port << std::endl;
					this->clientPorts.insert(sockaddr.sin_port);
				}
				else {
					Frame temp = Frame::fromByteArray(buffer);
					temp.source = sockaddr.sin_port; // ���֡����Դ
					this->receiveFrameQueue.push(temp);
				}
				isReceiveEmpty = false;
			}
			else {
				isReceiveEmpty = true;
			}
			//else {
			//	std::this_thread::sleep_for(std::chrono::milliseconds(20)); // ���߳�����20ms������ϵͳ����
			//}
			delete[] buffer;
		}
	}
};

