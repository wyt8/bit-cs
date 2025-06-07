#include<iostream>
using namespace std;

// 队列初始化时默认的大小
const int defaultSize = 100;
// 增大队列大小时的增量大小
const int increasementSize = 10;

class CLQueue
{
public:
    CLQueue();
    bool add(int num);
    bool remove();
    int get();
    bool isEmpty();
    bool isFull();
    // 析构函数释放动态申请的内存
    ~CLQueue() { delete[] queue; }
private:
    int* queue;
    // rear 指向队尾的上一个位置， front 指向队头的实际位置
    int rear, front;
    int maxSize;
};

CLQueue::CLQueue()
{
    queue = new int[defaultSize];
    // 初始时rear和front都指向队列的0位置
    rear = front = 0;
    maxSize = defaultSize;
}

bool CLQueue::isEmpty()
{
    if(rear == front)
        // 当队头指针和队尾指针指向同一个位置时表示空
        return true;
    return false;
}

bool CLQueue::isFull()
{
    if((rear + 1) % maxSize == front)
        // 当队尾指针加一和队头指针位置相同时表示队列满
        return true;
    return false;
}

bool CLQueue::add(int num)
{// 队尾增加元素
    if(isFull())
    {// 队列满扩充大小
        int* temp = new int[maxSize + increasementSize];
        if(!temp)
        {
            cout << "内存申请失败！" << endl;
            return false;
        }
        for(int i = 0; i < maxSize; i++)
        {
            temp[i] = queue[i];
        }
        maxSize = maxSize + increasementSize;
        delete[] queue;
        queue = temp;
    }

    queue[rear] = num;
    rear = (rear + 1) % maxSize;
    return true;
}

bool CLQueue::remove()
{// 队头移除元素
    if(!isEmpty())
    {// 队列不空时才可以移除队头元素
        front = (front + 1) % maxSize;
    }
    return false;
}

int CLQueue::get()
{
    return queue[front];
}

int main()
{
    CLQueue q;
    q.add(1);
    q.add(2);
    q.add(3);
    cout << q.get() << endl;
    q.remove();
    cout << q.get() << endl;
    system("pause");
    return 0;
}