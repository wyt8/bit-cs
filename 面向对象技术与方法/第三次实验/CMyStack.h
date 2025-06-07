#pragma once

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
    {
        double* temp;
        if(!(temp = new double[m_iCapacity + increasementStep]))
        {
            return false;
        }
        else
        {
            for(int i = 0; i < m_iSize; i++)
            {
                temp[i] = m_pTop[i];
            }
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
