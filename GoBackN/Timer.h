#pragma once
#include <map>
#include <thread>
#include "ThreadSafeQueue.h"
#include "global.h"
#include <chrono>

class Timer
{
public:
	Timer(ThreadSafeQueue<EventType>* eventQueue, int timeout): eventQueue(eventQueue), timeout(timeout) {
		std::thread t(&Timer::run, this); // ��ʼһ���߳������ж�ʱ
		t.detach();
	}

	void addTimer(int id) {
		this->addTimerQueue.push(id);
	}

	void  removeTimer(int id) {
		this->removeTimerQueue.push(id);
	}

private:
	int timeout;
	ThreadSafeQueue<int> addTimerQueue;
	ThreadSafeQueue<int> removeTimerQueue;
	ThreadSafeQueue<EventType>* eventQueue;
	void run() {
		std::map<int, long long> map;
		int timerId;
		while (true) {
			if (map.size() == 0 && this->removeTimerQueue.size() == 0 && this->addTimerQueue.size() == 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50)); // ���߳�����50ms������ϵͳ����
			}

			// ɾ����ʱ��
			if (this->removeTimerQueue.try_pop(timerId)) {
				map.erase(timerId);
			}


			// ʹ�õ���������
			for (auto it = map.begin(); it != map.end(); ) {
				auto now = std::chrono::system_clock::now();
				// ��ʱ���ת��Ϊʱ���
				auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
				if (timestamp - it->second > this->timeout) {
					map.erase(it->first);
					this->eventQueue->push(EventType::timeout);
				}
				++it; // ��������
			}

			// ��Ӷ�ʱ��
			if (this->addTimerQueue.try_pop(timerId)) {
				auto now = std::chrono::system_clock::now();
				// ��ʱ���ת��Ϊʱ���
				auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
				map.insert(std::map<int, UINT_PTR>::value_type(timerId, timestamp));
			}
		}
	}
};

