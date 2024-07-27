#pragma once
#include <vector>
#include <deque>
#include <queue>
#include <thread>

#include "Frame.h"
#include "Packet.h"
#include "ThreadSafeQueue.h"
#include "global.h"
#include "PhysicalLayer.h"
#include "Timer.h"
#include "Logger.h"

class DataLinkLayer
{
public:
	DataLinkLayer(ThreadSafeQueue<EventType>* eventQueue, int initSeqNo, int swSize, int timeout, PhysicalLayer* physicalLayer)
		: eventQueue(eventQueue), initSeqNo(initSeqNo), swSize(swSize), timeout(timeout), physicalLayer(physicalLayer), timer(eventQueue, timeout){
		// TODO：没有对空指针进行检查
		std::thread t(&DataLinkLayer::run, this);
		t.detach();
	}

	// 网络层向数据链路层添加包
	void send(Packet packet) {
		this->sendPacketQueue.push(packet);
	}
	
	// 网络层从数据链路层接收包
	bool receive(Packet& packet) {
		return this->receivePacketQueue.try_pop(packet);
	}

private:
	int initSeqNo; // 起始PDU的序号
	int swSize; // 发送窗口大小
	int timeout; // 超时定时器值，单位为毫秒


	std::deque<Frame> sendFrameQueue; // 发送方的帧队列
	int firstSeqNo = initSeqNo; // 滑动窗口最左边的序号
	int nextSeqNo = initSeqNo; // 下一个要发送的序号
	int currentSeqNo = initSeqNo; // 下一个加入发送方帧队列的序号
	int ackedSeqNo = this->decSeqNo(initSeqNo); // 发送方已经确认的帧序号
	int expectedSeqNo = initSeqNo; // 接收方期望接受到的帧序号

	ThreadSafeQueue<EventType>* eventQueue;
	ThreadSafeQueue<Packet> sendPacketQueue; // 要发送的包队列
	ThreadSafeQueue<Packet> receivePacketQueue; // 接收到的包队列
	PhysicalLayer* physicalLayer;

	Timer timer; // 定时器

	int incSeqNo(int seqNo) {
		if (seqNo - this->initSeqNo >= this->swSize) {
			seqNo = this->initSeqNo;
		}
		else {
			seqNo++;
		}
		return seqNo;
	}

	int decSeqNo(int seqNo) {
		if (seqNo == this->initSeqNo) {
			seqNo = this->initSeqNo + this->swSize;
		}
		else {
			seqNo--;
		}
		return seqNo;
	}

	void run() {
		bool isReceiveEmpty = false;
		while (true) {
			//if (this->sendFrameQueue.size() == 0 && this->sendPacketQueue.size() == 0 && isReceiveEmpty) {
			//	std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 让线程休眠100ms，降低系统开销
			//}

			// 从包队列取出包转换为帧加入到滑动窗口中
			// TODO：将包拆分为不同帧，这里假设一个包对应一个帧
			while (this->sendFrameQueue.size() < this->swSize) { // 当前滑动窗口没满，可以向滑动窗口中添加数据
				Packet packet;
				if (this->sendPacketQueue.try_pop(packet)) {
					int bufferLen = packet.getLength();
					char* buffer = new char[bufferLen];
					memset(buffer, 0, bufferLen);
					packet.getByteArray(buffer, &bufferLen);

					this->sendFrameQueue.push_back(Frame(FrameType::data, this->currentSeqNo, buffer, bufferLen));
					this->currentSeqNo = this->incSeqNo(this->currentSeqNo);
					delete[] buffer;
				}
				else {
					break;
				}
			}

			// 从滑动窗口中取出没有发送的帧向物理层发送
			if (firstSeqNo == nextSeqNo) { // 当未确认帧都确认后才进行发送
				bool flag = false;
				for (auto& frame : this->sendFrameQueue) {
					// 发送滑动窗口中所有没有发送的帧
					if (!flag && frame.seq == this->nextSeqNo) {
						flag = true;
						// 开始计时
						this->timer.addTimer(0);
					}
					if (flag) {
						std::string log = "[send] pdu_to_send=" + std::to_string(frame.seq) + ",status=New,ackedNo=" + std::to_string(this->ackedSeqNo);
						Logger::write(log);
						this->physicalLayer->send(frame);
						this->nextSeqNo = this->incSeqNo(this->nextSeqNo);
					}
				}
			}


			// 从物理层中取出帧进行处理
			Frame frame;
			if (this->physicalLayer->receive(frame)) {
				isReceiveEmpty = false;
				switch (frame.type) {
				case FrameType::data:
					// 判断从物理层接收到的帧是否符合预期
					if (frame.seq == this->expectedSeqNo) {
						// 向物理层发送确认帧或否者帧
						if (frame.isRight()) {
							std::string log = "[recv] pdu_exp=" + std::to_string(this->expectedSeqNo) + ",status=OK,pdu_recv=" + std::to_string(frame.seq);
							Logger::write(log);
							this->receivePacketQueue.push(Packet::fromByteArray(frame.data.data()));
							this->expectedSeqNo = this->incSeqNo(this->expectedSeqNo);
							this->physicalLayer->send(Frame(FrameType::ack, this->expectedSeqNo, nullptr, 0));
						}
						else {
							this->physicalLayer->send(Frame(FrameType::nak, this->expectedSeqNo, nullptr, 0));
							std::string log = "[recv] pdu_exp=" + std::to_string(this->expectedSeqNo) + ",status=DataErr,pdu_recv=" + std::to_string(frame.seq);
							Logger::write(log);
							std::cout << "[datalink] 接收到的帧校验错误" << std::endl;
						}
					}
					else {
						std::string log = "[recv] pdu_exp=" + std::to_string(this->expectedSeqNo) + ",status=NoErr,pdu_recv=" + std::to_string(frame.seq);
						Logger::write(log);
					}
					break;
				case FrameType::ack:
				{
					// 如果收到了所有已发送但没有确认帧的确认帧，则停止计时器
					if (frame.seq == nextSeqNo) {
						this->timer.removeTimer(0);
					}
					this->sendFrameQueue.pop_front();
					this->firstSeqNo = this->incSeqNo(this->firstSeqNo);
					this->ackedSeqNo = this->incSeqNo(this->ackedSeqNo);
					break;
				}
				case FrameType::nak:
				{
					// 停止计时器并开始新的计时器
					this->timer.removeTimer(0);
					this->timer.addTimer(0);
					for (auto& frame : this->sendFrameQueue) {
						// 发送滑动窗口中所有收到确认帧的帧
						if (frame.seq == this->nextSeqNo) {
							break;
						}
						this->physicalLayer->send(frame);
						std::string log = "[send] pdu_to_send=" + std::to_string(frame.seq) + ",status=RT,ackedNo=" + std::to_string(this->ackedSeqNo);
						Logger::write(log);
					}
					
					break;
				}
				default:
					std::cout << "[datalink][warning] 不期望的帧类型" << std::endl;
				}
			}
			else {
				isReceiveEmpty = true;
			}

			EventType eventType;
			if (this->eventQueue->try_pop(eventType)) {
				switch (eventType) {
				case EventType::timeout:
				{
					bool flag = true;
					for (auto& frame : this->sendFrameQueue) {
						// 发送滑动窗口中所有收到确认帧的帧
						if (frame.seq == this->nextSeqNo) {
							break;
						}
						if (flag) {
							this->timer.addTimer(0);
							std::cout << "[datalink] 超时" << std::endl;
						}
						flag = false;
						this->physicalLayer->send(frame);
						std::string log = "[send] pdu_to_send=" + std::to_string(frame.seq) + ",status=TO,ackedNo=" + std::to_string(this->ackedSeqNo);
						Logger::write(log);
					}
					break;
				}
				default:
					std::cout << "其他事件类型" << std::endl;
				}
			}
		}
	}
};

