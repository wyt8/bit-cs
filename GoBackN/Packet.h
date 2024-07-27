#pragma once
#include <vector>
#include <iostream>
#include "Config.h"

const int MAX_PACKET_LEGNTH = 10240;

enum PacketType {
	fileContent,
	fileStart,
	fileEnd,
	none
};

/*
* 包的结构
*
* 1字节类型：type
* 2字节数据大小：dataLen
* dataLen字节数据：data
*/

class Packet
{
public:
	Packet() : type(PacketType::none), dataLen(0) {}

	Packet(PacketType type, char* data, int dataLen) {
		this->type = type;
		this->dataLen = dataLen;
		for (int i = 0; i < dataLen; i++) {
			this->data.push_back(data[i]);
		}
	}

	void getByteArray(char* bytes, int* len) {
		if (*len < this->getLength()) {
			// 防止缓存区溢出
			*len = 0;
			return;
		}

		int index = 0;
		bytes[index++] = (this->type) & 255;
		bytes[index++] = this->dataLen & 255;
		bytes[index++] = (this->dataLen >> 8) & 255;

		for (int i = 0; i < this->dataLen; i++) {
			bytes[index++] = this->data[i];
		}

		*len = index;
	}

	static Packet fromByteArray(char* bytes) {
		int index = 0;
		Packet packet;
		packet.type = (PacketType)bytes[index++];
		packet.dataLen = *((unsigned short*)(bytes+index));
		index += 2;
		for (int i = 0; i < packet.dataLen; i++) {
			packet.data.push_back(bytes[index++]);
		}

		return packet;
	}

	int getLength() {
		return 3 + dataLen;
	}

	PacketType type;
	int dataLen;
	std::vector<char> data;
	unsigned short source;
};

