#include<iostream>
using namespace std;

const int increasementStep = 10;

class CMyStack
{
private:
    double *m_pTop; // Top pointer of stack
    int m_iSize; // Number of actual elements
    int m_iCapacity; // Capacity of stack
public:
    CMyStack(int size);
    ~CMyStack();
    double Pop(); 
    double Peek(); // Get top element
    bool Push(double ch);
    bool isEmpty(); // Stack is empty?
    bool isFull(); // Stack is full?
    int GetSize(); // Get number of actual elements
    void Clear(); // Clear stack
};

CMyStack::CMyStack(int size)
{
    m_pTop = new double[size];
    m_iSize = 0;
    m_iCapacity = size;
}

CMyStack::~CMyStack()
{
    delete[] m_pTop;
}

double CMyStack::Pop()
{
    if(isEmpty())
    // 为空时无法删除
        return 0;
    return m_pTop[--m_iSize];
}

double CMyStack::Peek()
{
    if(isEmpty())
        return 0;
    return m_pTop[m_iSize - 1];
}

bool CMyStack::Push(double ch)
{
    if(isFull())
    {// 栈满时重新申请内存
        double* temp;
        if(!(temp = new double[m_iCapacity + increasementStep]))
        {
            cout << "内存申请失败！" << endl;
            return false;
        }
        else
        {
            for(int i = 0; i < m_iSize; i++)
            {
                temp[i] = m_pTop[i];
            }
            // 注意要释放原来的内存空间
            delete[] m_pTop;
            m_pTop = temp;
            m_iCapacity += increasementStep;
            m_pTop[m_iSize++] = ch;
            return true;
        }

    }
    m_pTop[m_iSize++] = ch;
    return true;
}

bool CMyStack::isEmpty()
{
    if(m_iSize == 0)
        return true;
    return false;
}

bool CMyStack::isFull()
{
    if(m_iSize == m_iCapacity)
    {
        return true;
    }
    return false;
}

void CMyStack::Clear()
{
    m_iSize = 0;
}

int CMyStack::GetSize()
{
    return m_iSize;
}

int main()
{
    CMyStack cms = CMyStack(2);
    cms.Push(1.0);
    cms.Push(2.0);
    cout << cms.isFull() << endl;
    cms.Push(3.0);
    cout << cms.Pop() << endl;
    cout << cms.Pop() << endl;
    cout << cms.Pop() << endl;
    cout << cms.isEmpty() << endl;
    cms.Push(4.0);
    cms.Clear();
    cout << cms.isEmpty() << endl;
    system("pause");
    return 0;
}