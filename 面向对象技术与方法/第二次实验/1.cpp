#include<iostream>
using namespace std;

// ���ϳ�ʼ��ʱĬ�ϵĴ�С
const int defaultSize = 100;
// ���󼯺ϴ�Сʱ��������С
const int increasementSize = 100;

class CSet
{
public:
    // ����������ʼ������󣬼����а���������������Ԫ��
    CSet(int intArr[], int arrLen);
    // ��һ����������ʼ�����󣬼����н�����һ������
    CSet(int num);
    // Ĭ�Ϲ��캯��
    CSet();
    // ��������
    CSet(const CSet& anotherSet);
    bool isExist(int num) const;
    bool addItem(int num);
    bool removeItem(int num);
    bool isEqual(const CSet& anotherSet);
    CSet intersection(const CSet& anotherSet);
    CSet Union(const CSet& anotherSet);
    // ��������е�Ԫ��
    void printSet();
    // �����������ǵ�Ҫ�ͷŶ�̬������ڴ�
    ~CSet() { delete[] intArray; }
private:
    int* intArray;
    // ���Ͽ��������Ԫ�ظ���
    int maxSize;
    // ��ǰ���ϴ�С
    int size;
    // ��̬���Ӽ��ϴ�С������Ϊ�������ϴ�С
    bool increaseSize(int expectedSize);
};

CSet::CSet()
{
    maxSize = defaultSize;
    intArray = new int[maxSize];
    size = 0;
}

CSet::CSet(const CSet& anotherSet)
{
    size = anotherSet.size;
    maxSize = anotherSet.maxSize;
    intArray = new int[maxSize];
    for(int i = 0; i < size; i++)
    {
        intArray[i] = anotherSet.intArray[i];
    }
}

CSet::CSet(int num)
{   
    maxSize = defaultSize;
    intArray = new int[maxSize];
    size = 1;
    intArray[0] = num;
}

CSet::CSet(int intArr[], int arrLen)
{
    if(arrLen <= defaultSize)
    {   // ��������鳤��С��Ĭ�ϵļ��ϴ�Сʱ
        maxSize = defaultSize;
    }
    else
    {
        maxSize = defaultSize + increasementSize;
    }
    intArray = new int[maxSize];
    // ��ǰ size ��СΪ0
    size = 0;
    for(int i = 0; i < arrLen; i++)
    {
        if(!isExist(intArr[i]))
        {
            intArray[size++] = intArr[i];
        }
    }
}

bool CSet::isExist(int num) const
{
    for(int i = 0; i < size; i++)
    {
        if(intArray[i] == num)
        {
            return true;
        }
    }
    return false;
}

bool CSet::addItem(int num)
{
    if(!isExist(num))
    {   // ��Ҫ�����Ԫ���ڼ����в�����ʱ�Ž��м���
        if(size < maxSize)
        {   // ���ϴ�С�㹻����Ԫ��ʱֱ�ӽ��м���
            intArray[size++] = num;
            return true;
        }
        else
        {   // ���ϴ�С����ʱ�����ڴ�
            if(increaseSize(size + 1))
            {
                intArray[size++] = num;
                return true;
            }
            else
                return false;

        }
    }
    return true;
}

bool CSet::increaseSize(int expectedSize)
{
    if(expectedSize <= maxSize)
    {
        return true;
    }
    else
    {
        int* temp = new int[expectedSize + increasementSize];
        if(!temp)
        {
            cout << "�ڴ�����ʧ��" << endl;
            return false;
        }
        for(int i = 0; i < size; i++)
        {
            temp[i] = intArray[i];
        }
        maxSize = expectedSize + increasementSize;
        // ע��Ҫ�ͷ�ԭ��������ڴ�
        delete[] intArray;
        intArray = temp;
        return true;
    }
}

bool CSet::removeItem(int num)
{
    if(isExist(num))
    {
        for(int i = 0; i < size; i++)
        {
            if(intArray[i] == num)
            {   // ���������Ҫɾ����Ԫ���ü����е����һ��Ԫ�ظ���
                intArray[i] = intArray[size - 1];
                size--;
                return true;
            }
        }
    }
    return false;
}

bool CSet::isEqual(const CSet& anotherSet)
{
    if(size == anotherSet.size)
    {
        for(int i = 0; i < size; i++)
        {
            if(!anotherSet.isExist(intArray[i]))
                // ��һ�������е�Ԫ������һ�������в�����ʱ���������ϲ�ͬ
                return false;
        }
        return true;
    }
    // ���ϴ�С��ͬ���򼯺ϱض���ͬ
    return false;
}

CSet CSet::Union(const CSet& anotherSet)
{
    CSet res;
    for(int i = 0; i < size; i++)
    {
        if(anotherSet.isExist(intArray[i]))
        {
            res.addItem(intArray[i]);
        }
    }
    return res;
}

CSet CSet::intersection(const CSet& anotherSet)
{
    CSet res(*this);
    for(int i = 0; i < anotherSet.size; i++)
    {
        if(!res.isExist(anotherSet.intArray[i]))
        {
            res.addItem(anotherSet.intArray[i]);
        }
    }
    return res;
}

void CSet::printSet()
{
    for(int i = 0; i < size; i++)
    {
        cout << intArray[i] << " "; 
    }
    cout << endl;
}

int main()
{
    // ���н�����£�
    // s1Ϊ��1
    // s2Ϊ��1 2 3 4 5 6 7 10
    // s2���Ƿ����10:1
    // s1��s2�Ĳ�����1 2 3 4 5 6 7 10
    // s1�в���1��1
    // s1�в���2��1 2
    // s1��s2�Ľ�����1 2
    // s2�Ƴ�3��4��5��6��7��10��1 2
    // s1��s2�Ƿ���ȣ�1
    
    CSet s1(1);
    cout << "s1Ϊ��";
    s1.printSet();

    int nums[10] = {1, 2, 3, 4, 5, 6, 7, 10, 1, 2};
    CSet s2(nums, sizeof(nums) / sizeof(int));
    cout << "s2Ϊ��";
    s2.printSet();

    cout << "s2���Ƿ����10:" << s2.isExist(10) << endl;

    cout << "s1��s2�Ĳ�����";
    s1.intersection(s2).printSet();

    s1.addItem(1);
    cout << "s1�в���1��";
    s1.printSet();

    cout << "s1�в���2��";
    s1.addItem(2);
    s1.printSet();

    cout << "s1��s2�Ľ�����";
    s1.Union(s2).printSet();

    cout << "s2�Ƴ�3��4��5��6��7��10��";
    s2.removeItem(3), s2.removeItem(4), s2.removeItem(5), s2.removeItem(6), s2.removeItem(7), s2.removeItem(10);
    s2.printSet();

    cout << "s1��s2�Ƿ���ȣ�" << s1.isEqual(s2) << endl;

    system("pause");
    return 0;
}