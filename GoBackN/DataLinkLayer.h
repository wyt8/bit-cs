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
		// TODO��û�жԿ�ָ����м��
		std::thread t(&DataLinkLayer::run, this);
		t.detach();
	}

	// �������������·����Ӱ�
	void send(Packet packet) {
		this->sendPacketQueue.push(packet);
	}
	
	// ������������·����հ�
	bool receive(Packet& packet) {
		return this->receivePacketQueue.try_pop(packet);
	}

private:
	int initSeqNo; // ��ʼPDU�����
	int swSize; // ���ʹ��ڴ�С
	int timeout; // ��ʱ��ʱ��ֵ����λΪ����


	std::deque<Frame> sendFrameQueue; // ���ͷ���֡����
	int firstSeqNo = initSeqNo; // ������������ߵ����
	int nextSeqNo = initSeqNo; // ��һ��Ҫ���͵����
	int currentSeqNo = initSeqNo; // ��һ�����뷢�ͷ�֡���е����
	int ackedSeqNo = this->decSeqNo(initSeqNo); // ���ͷ��Ѿ�ȷ�ϵ�֡���
	int expectedSeqNo = initSeqNo; // ���շ��������ܵ���֡���

	ThreadSafeQueue<EventType>* eventQueue;
	ThreadSafeQueue<Packet> sendPacketQueue; // Ҫ���͵İ�����
	ThreadSafeQueue<Packet> receivePacketQueue; // ���յ��İ�����
	PhysicalLayer* physicalLayer;

	Timer timer; // ��ʱ��

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
			//	std::this_thread::sleep_for(std::chrono::milliseconds(100)); // ���߳�����100ms������ϵͳ����
			//}

			// �Ӱ�����ȡ����ת��Ϊ֡���뵽����������
			// TODO���������Ϊ��ͬ֡���������һ������Ӧһ��֡
			while (this->sendFrameQueue.size() < this->swSize) { // ��ǰ��������û���������򻬶��������������
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

			// �ӻ���������ȡ��û�з��͵�֡������㷢��
			if (firstSeqNo == nextSeqNo) { // ��δȷ��֡��ȷ�Ϻ�Ž��з���
				bool flag = false;
				for (auto& frame : this->sendFrameQueue) {
					// ���ͻ�������������û�з��͵�֡
					if (!flag && frame.seq == this->nextSeqNo) {
						flag = true;
						// ��ʼ��ʱ
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


			// ���������ȡ��֡���д���
			Frame frame;
			if (this->physicalLayer->receive(frame)) {
				isReceiveEmpty = false;
				switch (frame.type) {
				case FrameType::data:
					// �жϴ��������յ���֡�Ƿ����Ԥ��
					if (frame.seq == this->expectedSeqNo) {
						// ������㷢��ȷ��֡�����֡
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
							std::cout << "[datalink] ���յ���֡У�����" << std::endl;
						}
					}
					else {
						std::string log = "[recv] pdu_exp=" + std::to_string(this->expectedSeqNo) + ",status=NoErr,pdu_recv=" + std::to_string(frame.seq);
						Logger::write(log);
					}
					break;
				case FrameType::ack:
				{
					// ����յ��������ѷ��͵�û��ȷ��֡��ȷ��֡����ֹͣ��ʱ��
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
					// ֹͣ��ʱ������ʼ�µļ�ʱ��
					this->timer.removeTimer(0);
					this->timer.addTimer(0);
					for (auto& frame : this->sendFrameQueue) {
						// ���ͻ��������������յ�ȷ��֡��֡
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
					std::cout << "[datalink][warning] ��������֡����" << std::endl;
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
						// ���ͻ��������������յ�ȷ��֡��֡
						if (frame.seq == this->nextSeqNo) {
							break;
						}
						if (flag) {
							this->timer.addTimer(0);
							std::cout << "[datalink] ��ʱ" << std::endl;
						}
						flag = false;
						this->physicalLayer->send(frame);
						std::string log = "[send] pdu_to_send=" + std::to_string(frame.seq) + ",status=TO,ackedNo=" + std::to_string(this->ackedSeqNo);
						Logger::write(log);
					}
					break;
				}
				default:
					std::cout << "�����¼�����" << std::endl;
				}
			}
		}
	}
};

