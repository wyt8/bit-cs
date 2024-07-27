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
		std::thread t(&Timer::run, this); // 开始一个线程来进行定时
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
				std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 让线程休眠50ms，降低系统开销
			}

			// 删除定时器
			if (this->removeTimerQueue.try_pop(timerId)) {
				map.erase(timerId);
			}


			// 使用迭代器遍历
			for (auto it = map.begin(); it != map.end(); ) {
				auto now = std::chrono::system_clock::now();
				// 将时间点转换为时间戳
				auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
				if (timestamp - it->second > this->timeout) {
					map.erase(it->first);
					this->eventQueue->push(EventType::timeout);
				}
				++it; // 继续遍历
			}

			// 添加定时器
			if (this->addTimerQueue.try_pop(timerId)) {
				auto now = std::chrono::system_clock::now();
				// 将时间点转换为时间戳
				auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
				map.insert(std::map<int, UINT_PTR>::value_type(timerId, timestamp));
			}
		}
	}
};

