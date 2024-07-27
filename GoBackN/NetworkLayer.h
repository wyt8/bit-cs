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
		// TODO��û�жԿ�ָ����м��
		std::thread t(&NetworkLayer::run, this);
		t.detach();
	}

	// ���Ҫ���͵��ļ�·��
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
		return std::string(); // ���û����չ�������ؿ��ַ���
	}

	void run() {
		while (true) {
			if (!this->sendingFile.is_open() && !this->receivingFile.is_open()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); // ���߳�����100ms������ϵͳ����
			}

			if (!this->sendingFile.is_open()) {
				while (true) { // ѭ���ҵ��ļ�·�������п��Դ򿪵��ļ�
					std::string path;
					if (!this->filePathQueue.try_pop(path)) { // ��ǰ�ļ�·��������û�����ݣ��˳�ѭ��
						break;
					}
					this->sendingFile.open(path, std::ios::binary | std::ios::in);
					if (!this->sendingFile.is_open()) {
						std::cerr << "[network][error] �ļ���ʧ�ܣ�" << path << std::endl;
					}
					else {
						std::cout << "[network] ��ʼ�����ļ���" << path << std::endl;
						std::string ext = this->getFileExtension(path);
						// ����Ϊ��ʼ�İ�������Ϊ�ļ�����
						this->dataLinkLayer->send(Packet(PacketType::fileStart, (char*)ext.c_str(), ext.size()));
						break;
					}
				}
			}

			// ��������·�㷢��һ����
			if (this->sendingFile.is_open()) {
				int bufferLen = Config::DataSize;
				char* buffer = new char[bufferLen];
				memset(buffer, 0, bufferLen);
				this->sendingFile.read(buffer, bufferLen);
				if (this->sendingFile.gcount() == 0) {
					this->dataLinkLayer->send(Packet(PacketType::fileEnd, nullptr, 0));
					std::cout << "[network] ���������ļ�" << std::endl;
					this->sendingFile.close();
				}
				else {
					this->dataLinkLayer->send(Packet(PacketType::fileContent, buffer, this->sendingFile.gcount()));
				}
				delete[] buffer;
			}

			// ��������·��ȡ��һ�������д���
			Packet packet;
			if (this->dataLinkLayer->receive(packet)) {
				switch (packet.type) {
				case PacketType::fileStart:
				{
					// ʹ��system_clock��ȡ��ǰʱ���
					auto now = std::chrono::system_clock::now();
					// ��ʱ���ת��Ϊʱ���
					auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
					std::string fileName = std::to_string(timestamp);
					if (packet.dataLen > 0) {
						fileName = fileName + "." + std::string(packet.data.begin(), packet.data.end());
					}
					if (this->receivingFile.is_open()) {
						this->receivingFile.close();
					}
					this->receivingFile.open(fileName, std::ios::binary | std::ios::out);
					std::cout << "[network] ��ʼ�����ļ���" << fileName << std::endl;
					break;
				}
				case PacketType::fileContent:
					if (this->receivingFile.is_open()) {
						this->receivingFile.write(packet.data.data(), packet.dataLen);
						if (this->receivingFile.fail()) {
							std::cerr << "[network][error] �ļ�д��ʧ��" << std::endl;
						}
					}
					break;
				case PacketType::fileEnd:
					if (this->receivingFile.is_open()) {
						std::cout << "[network] �����ļ����" << std::endl;
						this->receivingFile.close();
					}
					break;
				default:
					std::cout << "[network][warning] ����İ�����" << std::endl;
				}
			}
		}
	}
};
