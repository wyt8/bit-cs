#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

/*
* 线程安全的队列
* C++中的queue是线程不安全的，这里使用互斥量和条件变量来实现线程安全的队列
*/
template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> q;
    std::mutex mut;
    std::condition_variable cond;

public:
    void push(T new_value);

    // 如果队列为空则会阻塞
    void wait_and_pop(T& value);

    // 非阻塞的，如果队列为空则返回false
    bool try_pop(T& value);

    int size();
};

template<typename T>
void ThreadSafeQueue<T>::push(T new_value) {
    std::lock_guard<std::mutex> lock(mut);
    q.push(new_value);
    cond.notify_one();
}

template<typename T>
void ThreadSafeQueue<T>::wait_and_pop(T& value) {
    std::unique_lock<std::mutex> lock(mut);
    cond.wait(lock, [&] { return !q.empty(); });
    value = q.front();
    q.pop();
}

template<typename T>
bool ThreadSafeQueue<T>::try_pop(T& value) {
    std::lock_guard<std::mutex> lock(mut);
    if (q.empty()) return false;
    value = q.front();
    q.pop();
    return true;
}

template<typename T>
int ThreadSafeQueue<T>::size() {
    std::lock_guard<std::mutex> lock(mut);
    auto size = this->q.size();
    return size;
}
