#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

/*
* �̰߳�ȫ�Ķ���
* C++�е�queue���̲߳���ȫ�ģ�����ʹ�û�����������������ʵ���̰߳�ȫ�Ķ���
*/
template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> q;
    std::mutex mut;
    std::condition_variable cond;

public:
    void push(T new_value);

    // �������Ϊ���������
    void wait_and_pop(T& value);

    // �������ģ��������Ϊ���򷵻�false
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
