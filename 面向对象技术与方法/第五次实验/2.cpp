#include<iostream>
using namespace std;

const int initSize = 50;

template<typename T>
class CList
{
public:
    CList()
    {
        list = new T[initSize];
        size = 0;
        maxLength = initSize;
    }
    CList(unsigned s)
    {
        list = new T[s];
        size = 0;
        maxLength = s;
    }
    bool insert(unsigned pos,T element);
    bool Add(T& element);
    bool Delete(unsigned pos);
    T& operator[] (unsigned index);
    ~CList()
    {
        delete[] list;
    }
private:
    int maxLength;
    int size;
    T* list;
};

template<typename T>
bool CList<T>::Add(T& element)
{
    if(size == maxLength)
    {// ����ռ�������ʱ��������Ԫ��
        return false;
    }
    list[size++] = element;
    return true;
}

template<typename T>
bool CList<T>::insert(unsigned pos, T element)
{
    if(size == maxLength)
    {// ��������ʱ���ܼ�������Ԫ��
        return false
        throw "capility is full";
    }
    if(pos < 0 || pos > maxLength)
    {// �����pos���Ϸ�ʱ���ܲ���
        return false
        throw "insert position is illegal!";
    }
    for(int i = size; i > pos; i--)
    {
        list[i] = list[i-1];
    }
    list[pos] = T;
    size++;
    return true;
}

template<typename T>
T& CList<T>::operator[](unsigned index)
{
    if(index >= maxLength)
    {// �±겻�Ϸ�ʱ�׳��쳣
        throw "index beyond maxLenth";
    }
    return list[index];
}

template<typename T>
bool CList<T>::Delete(unsigned pos)
{
    if(size == 0)
    {// Ϊ��ʱʱ����ɾ��Ԫ��
        return false
        throw "capility is empty";
    }
    if(pos < 0 || pos > maxLength)
    {// �����pos���Ϸ�ʱ���ܲ���
        return false
        throw "insert position is illegal!";
    }
    for(int i = pos; i < size; i++)
    {
        list[i] = list[i+1];
    }
    size--;
    return true;
}



class CStudent
{
public:
    // Ĭ�ϲ���
    CStudent(string name ="", int age = 0): m_name(name), m_age(age)
    {}

    bool operator== (CStudent cStudent)
    {
        if(m_name == cStudent.m_name && m_age == cStudent.m_age)
        {
            return true;
        }
        return false;
    }
private:
    string m_name;
    int m_age;
};

int main()
{
    CStudent s1("Joan", 22), s2("John", 19);
    CList<CStudent> listStudent(50); // 50 is capcity of List
    listStudent.Add(s1);
    listStudent.Add(s2);
    if (listStudent[0] == listStudent[1]) // Two students have same age.
    cout << "Equal." << endl;
    else
    cout << "Not equal." << endl;
    system("pause");
    return 0;
}