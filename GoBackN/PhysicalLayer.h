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

//加载 ws2_32.dll
#pragma comment(lib, "ws2_32.lib")

/*
* 物理层
* 实际采用UDP Socket来进行模拟并实现Frame的发送和接收，每个UDP数据报封装一个Frame
* 所以这里就不需要进行分帧等操作，Frame中也不用包含帧的起始标志和结束标志
*/
class PhysicalLayer
{
public:
	PhysicalLayer(ThreadSafeQueue<EventType>* eventQueue, unsigned short serverPort, int errorRate, int lostRate)
		: eventQueue(eventQueue), errorRate(errorRate), lostRate(lostRate) {
		bool isServer = false;
		// 通过互斥信号量来判断该程序是否该为服务端
		HANDLE mutexHandle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"gobackn");
		if (mutexHandle == NULL) {
			mutexHandle = CreateMutex(NULL, FALSE, L"gobackn");
			this->server = true;
		}
		else {
			this->server = false;
		}

		// 设置服务端地址结构
		memset(&this->serverAddr, 0, sizeof(this->serverAddr));
		this->serverAddr.sin_family = AF_INET;
		this->serverAddr.sin_port = serverPort;
		inet_pton(this->serverAddr.sin_family, "127.0.0.1", &this->serverAddr.sin_addr.s_addr);

		// 进行初始化
		this->inited = this->init();

		// 开启线程模拟物理层收发消息
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
			// 关闭套接字
			closesocket(this->sockfd);
			WSACleanup();
		}
	}

	// 是否为服务端
	bool isServer() const {
		return this->server;
	}
	
	// 向物理层添加要发送的帧
	void send(Frame frame) {
		this->sendFrameQueue.push(frame);
	}

	// 获取物理层接收到的帧
	bool receive(Frame& frame) {
		return this->receiveFrameQueue.try_pop(frame);
	}

private:
	bool init() {
		// 初始化Winsock
		if (WSAStartup(MAKEWORD(2, 2), &this->wsaData) != 0) {
			std::cerr << "WSAStartup failed" << std::endl;
			return false;
		}

		// 创建套接字
		this->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sockfd == INVALID_SOCKET) {
			std::cerr << "Socket creation failed" << std::endl;
			WSACleanup();
			return false;
		}

		u_long iMODE = 1; // 1为非阻塞，0为阻塞
		ioctlsocket(this->sockfd, FIONBIO, &iMODE); // 设置recvfrom是否为阻塞

		// 服务端需要绑定端口；而客户端不需要绑定端口，但需要向服务器发送消息让服务器记住客户端
		if (this->server) {
			// 绑定套接字
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

	bool server; // 是否为服务端
	bool inited; // 是否进行了初始化
	int errorRate; // 错误率
	int lostRate; // 丢失率
	SOCKET sockfd;
	WSADATA wsaData;
	sockaddr_in serverAddr;
	ThreadSafeQueue<EventType>* eventQueue; // 事件队列

	ThreadSafeQueue<Frame> sendFrameQueue; // 要发送的帧队列
	ThreadSafeQueue<Frame> receiveFrameQueue; // 接收到的帧队列

	std::unordered_set<unsigned short> clientPorts; // 客户端端口集合，仅在服务端使用

	void run() {
		bool isReceiveEmpty = false;
		while (true) {
			//if (sendFrameQueue.size() == 0 && isReceiveEmpty) {
			//	std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 让线程休眠100ms，降低系统开销
			//}

			// 从要发送的帧队列中取得帧发送
			Frame frame;
			if (this->sendFrameQueue.try_pop(frame)) {
				if (frame.type == FrameType::data) {
					// 模拟丢失
					if (this->lostRate > 0) {
						// 初始化随机数生成器
						std::random_device rd;  // 用于获得种子
						std::mt19937 gen(rd()); // 使用随机设备种子初始化Mersenne Twister生成器
						std::uniform_int_distribution<> distrib(1, this->lostRate); // 定义分布范围

						// 获取一个随机数
						int randomNumber = distrib(gen);

						if (randomNumber == 1) {
							std::cout << "[physical] 帧丢失" << std::endl;
							goto sendFinish;
						}
					}
				}
				

				int len = frame.getLength();
				char* buffer = new char[len];
				memset(buffer, 0, len);
				frame.getByteArray(buffer, &len);
				
				// 模拟错误
				if (this->errorRate > 0 && frame.type==FrameType::data) {
					// 初始化随机数生成器
					std::random_device rd;  // 用于获得种子
					std::mt19937 gen(rd()); // 使用随机设备种子初始化Mersenne Twister生成器
					std::uniform_int_distribution<> distrib(1, this->errorRate); // 定义分布范围

					// 获取一个随机数
					int randomNumber = distrib(gen);

					if (randomNumber == 1) {
						buffer[2] = 'w';
						std::cout << "[physical] 帧出错" << std::endl;
					}
				}

				if (this->server) {
					// 服务端向所有客户端发送该帧
					for (auto& clientPort : this->clientPorts) {
						sockaddr_in clientAddr;
						memset(&clientAddr, 0, sizeof(clientAddr));
						clientAddr.sin_family = AF_INET;
						clientAddr.sin_port = clientPort;
						inet_pton(clientAddr.sin_family, "127.0.0.1", &clientAddr.sin_addr.s_addr);
						sendto(this->sockfd, buffer, len, 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
						//std::cout << "[physical] 发送帧，到端口：" << clientPort << std::endl;
					}
				}
				else {
					// 客户端一定向服务器发送消息
					sendto(this->sockfd, buffer, len, 0, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr));
					//std::cout << "[physical] 发送帧，到端口：" << (unsigned short)this->serverAddr.sin_port << std::endl;
				}
				delete[] buffer;
			}
			//else {
			//	std::this_thread::sleep_for(std::chrono::milliseconds(20)); // 让线程休眠20ms，降低系统开销
			//}

		sendFinish:

			// 接收帧存储到接收帧队列中
			int len = MAX_FRAME_LENGTH;
			char* buffer = new char[len];
			memset(buffer, 0, len);
			sockaddr_in sockaddr;
			int socklen = sizeof(sockaddr);
			memset(&sockaddr, 0, sizeof(sockaddr));
			if (recvfrom(this->sockfd, buffer, len, 0, (struct sockaddr*)&sockaddr, &socklen) > 0) {
				if (this->server && strcmp("client", buffer) == 0) {
					std::cout << "[info] 发现新的客户端，端口为：" << sockaddr.sin_port << std::endl;
					this->clientPorts.insert(sockaddr.sin_port);
				}
				else {
					Frame temp = Frame::fromByteArray(buffer);
					temp.source = sockaddr.sin_port; // 标记帧的来源
					this->receiveFrameQueue.push(temp);
				}
				isReceiveEmpty = false;
			}
			else {
				isReceiveEmpty = true;
			}
			//else {
			//	std::this_thread::sleep_for(std::chrono::milliseconds(20)); // 让线程休眠20ms，降低系统开销
			//}
			delete[] buffer;
		}
	}
};

