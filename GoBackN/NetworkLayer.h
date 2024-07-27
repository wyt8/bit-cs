#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>

#include "ThreadSafeQueue.h"
#include "Packet.h"
#include "global.h"
#include "DataLinkLayer.h"

class NetworkLayer
{
public:
	NetworkLayer(ThreadSafeQueue<EventType>* eventQueue, DataLinkLayer* dataLinkLayer)
		: eventQueue(eventQueue), dataLinkLayer(dataLinkLayer) {
		// TODO：没有对空指针进行检查
		std::thread t(&NetworkLayer::run, this);
		t.detach();
	}

	// 添加要发送的文件路径
	void addFilePath(std::string filePath) {
		this->filePathQueue.push(filePath);
	}

private:
	ThreadSafeQueue<EventType>* eventQueue;
	ThreadSafeQueue<std::string> filePathQueue;
	DataLinkLayer* dataLinkLayer;
	std::ifstream sendingFile;
	std::ofstream receivingFile;

	std::string getFileExtension(const std::string& filePath) {
		if (filePath.find_last_of('.') != std::string::npos) {
			return filePath.substr(filePath.find_last_of('.') + 1);
		}
		return std::string(); // 如果没有扩展名，返回空字符串
	}

	void run() {
		while (true) {
			if (!this->sendingFile.is_open() && !this->receivingFile.is_open()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 让线程休眠100ms，降低系统开销
			}

			if (!this->sendingFile.is_open()) {
				while (true) { // 循环找到文件路径队列中可以打开的文件
					std::string path;
					if (!this->filePathQueue.try_pop(path)) { // 当前文件路径队列中没有数据，退出循环
						break;
					}
					this->sendingFile.open(path, std::ios::binary | std::ios::in);
					if (!this->sendingFile.is_open()) {
						std::cerr << "[network][error] 文件打开失败：" << path << std::endl;
					}
					else {
						std::cout << "[network] 开始传输文件：" << path << std::endl;
						std::string ext = this->getFileExtension(path);
						// 类型为开始的包其数据为文件类型
						this->dataLinkLayer->send(Packet(PacketType::fileStart, (char*)ext.c_str(), ext.size()));
						break;
					}
				}
			}

			// 向数据链路层发送一个包
			if (this->sendingFile.is_open()) {
				int bufferLen = Config::DataSize;
				char* buffer = new char[bufferLen];
				memset(buffer, 0, bufferLen);
				this->sendingFile.read(buffer, bufferLen);
				if (this->sendingFile.gcount() == 0) {
					this->dataLinkLayer->send(Packet(PacketType::fileEnd, nullptr, 0));
					std::cout << "[network] 结束传输文件" << std::endl;
					this->sendingFile.close();
				}
				else {
					this->dataLinkLayer->send(Packet(PacketType::fileContent, buffer, this->sendingFile.gcount()));
				}
				delete[] buffer;
			}

			// 从数据链路层取出一个包进行处理
			Packet packet;
			if (this->dataLinkLayer->receive(packet)) {
				switch (packet.type) {
				case PacketType::fileStart:
				{
					// 使用system_clock获取当前时间点
					auto now = std::chrono::system_clock::now();
					// 将时间点转换为时间戳
					auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
					std::string fileName = std::to_string(timestamp);
					if (packet.dataLen > 0) {
						fileName = fileName + "." + std::string(packet.data.begin(), packet.data.end());
					}
					if (this->receivingFile.is_open()) {
						this->receivingFile.close();
					}
					this->receivingFile.open(fileName, std::ios::binary | std::ios::out);
					std::cout << "[network] 开始接收文件：" << fileName << std::endl;
					break;
				}
				case PacketType::fileContent:
					if (this->receivingFile.is_open()) {
						this->receivingFile.write(packet.data.data(), packet.dataLen);
						if (this->receivingFile.fail()) {
							std::cerr << "[network][error] 文件写入失败" << std::endl;
						}
					}
					break;
				case PacketType::fileEnd:
					if (this->receivingFile.is_open()) {
						std::cout << "[network] 接收文件完毕" << std::endl;
						this->receivingFile.close();
					}
					break;
				default:
					std::cout << "[network][warning] 意外的包类型" << std::endl;
				}
			}
		}
	}
};
