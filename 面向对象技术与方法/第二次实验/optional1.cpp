#include<iostream>
using namespace std;

// ���г�ʼ��ʱĬ�ϵĴ�С
const int defaultSize = 100;
// ������д�Сʱ��������С
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
    // ���������ͷŶ�̬������ڴ�
    ~CLQueue() { delete[] queue; }
private:
    int* queue;
    // rear ָ���β����һ��λ�ã� front ָ���ͷ��ʵ��λ��
    int rear, front;
    int maxSize;
};

CLQueue::CLQueue()
{
    queue = new int[defaultSize];
    // ��ʼʱrear��front��ָ����е�0λ��
    rear = front = 0;
    maxSize = defaultSize;
}

bool CLQueue::isEmpty()
{
    if(rear == front)
        // ����ͷָ��Ͷ�βָ��ָ��ͬһ��λ��ʱ��ʾ��
        return true;
    return false;
}

bool CLQueue::isFull()
{
    if((rear + 1) % maxSize == front)
        // ����βָ���һ�Ͷ�ͷָ��λ����ͬʱ��ʾ������
        return true;
    return false;
}

bool CLQueue::add(int num)
{// ��β����Ԫ��
    if(isFull())
    {// �����������С
        int* temp = new int[maxSize + increasementSize];
        if(!temp)
        {
            cout << "�ڴ�����ʧ�ܣ�" << endl;
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
{// ��ͷ�Ƴ�Ԫ��
    if(!isEmpty())
    {// ���в���ʱ�ſ����Ƴ���ͷԪ��
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