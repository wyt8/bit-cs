#pragma once
#include <vector>
#include <iostream>
#include "Config.h"

enum FrameType {
	data, ack, nak
};

const int MAX_FRAME_LENGTH = 10240;

/*
* 帧的结构
* 
* 1字节类型：type
* 1字节序号：seq
* 2字节数据大小：dataLen
* dataLen字节数据：data
* 2字节校验码：checksum
*/


class Frame
{
public:
	Frame():type(FrameType::data), seq(0), dataLen(0) {
		this->checksum = this->crc_ccitt();
	}

	Frame(FrameType type, int seq, char* data, int dataLen) {
		this->type = type;
		this->seq = seq;
		this->dataLen = dataLen;
		for (int i = 0; i < dataLen; i++) {
			this->data.push_back(data[i]);
		}
		this->checksum = this->crc_ccitt();
	}

	void getByteArray(char* bytes, int* len) {
		if (*len < this->getLength()) {
			// 防止缓存区溢出
			*len = 0;
			return;
		}
		int index = 0;
		bytes[index++] = (this->type) & 0xFF;
		bytes[index++] = (this->seq) & 0xFF;
		bytes[index++] = this->dataLen & 0xFF;
		bytes[index++] = (this->dataLen >> 8) & 0xFF;

		for (int i = 0; i < this->dataLen; i++) {
			bytes[index++] = this->data[i];
		}

		bytes[index++] = this->checksum & 0xFF;
		bytes[index++] = (this->checksum >> 8) & 0xFF;
		*len = index;
	}

	static Frame fromByteArray(char* bytes) {
		int index = 0;
		Frame frame;
		frame.type = (FrameType)bytes[index++];
		frame.seq = bytes[index++];
		frame.dataLen = *((unsigned short*)(bytes+index));
		index += 2;
		for (int i = 0; i < frame.dataLen; i++) {
			frame.data.push_back(bytes[index++]);
		}

		frame.checksum = *((unsigned short*)(bytes+index));
		index += 2;
		return frame;
	}

	// 获取该帧对应的字节数组长度
	int getLength() {
		return 6 + dataLen;
	}

	// 根据循环校验码判断该帧是否出错
	bool isRight() {
		return this->crc_ccitt() == this->checksum;
	}

	FrameType type;
	unsigned char seq;
	unsigned short source;
	unsigned short dataLen;
	std::vector<char> data;
	unsigned short checksum; // 采用CRC-CCITT标准

private:
	unsigned short crc_ccitt() {
		const unsigned short polynomial = 0x1021; // 循环校验码对应的多项式：x^16+x^12+x^5+1
		unsigned short crc = 0xFFFF;

		crc ^= (unsigned short)this->type << 8;
		for (int i = 0; i < 8; ++i) {
			if (crc & 0x8000) {
				crc = (crc << 1) ^ polynomial;
			}
			else {
				crc <<= 1;
			}
		}
		crc ^= (unsigned short)this->seq << 8;
		for (int i = 0; i < 8; ++i) {
			if (crc & 0x8000) {
				crc = (crc << 1) ^ polynomial;
			}
			else {
				crc <<= 1;
			}
		}
		crc ^= (unsigned short)(this->dataLen && 0xFF) << 8;
		for (int i = 0; i < 8; ++i) {
			if (crc & 0x8000) {
				crc = (crc << 1) ^ polynomial;
			}
			else {
				crc <<= 1;
			}
		}

		crc ^= (unsigned short)(this->dataLen >> 8 && 0xFF) << 8;
		for (int i = 0; i < 8; ++i) {
			if (crc & 0x8000) {
				crc = (crc << 1) ^ polynomial;
			}
			else {
				crc <<= 1;
			}
		}
		

		for (auto byte : this->data) {
			crc ^= (unsigned short)byte << 8;
			for (int i = 0; i < 8; ++i) {
				if (crc & 0x8000) {
					crc = (crc << 1) ^ polynomial;
				}
				else {
					crc <<= 1;
				}
			}
		}

		return crc;
	}
};
