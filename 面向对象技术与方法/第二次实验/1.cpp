#include<iostream>
using namespace std;

// 集合初始化时默认的大小
const int defaultSize = 100;
// 增大集合大小时的增量大小
const int increasementSize = 100;

class CSet
{
public:
    // 用数组来初始化类对象，集合中包含该数组中所有元素
    CSet(int intArr[], int arrLen);
    // 用一个整数来初始化对象，集合中仅包含一个整数
    CSet(int num);
    // 默认构造函数
    CSet();
    // 拷贝函数
    CSet(const CSet& anotherSet);
    bool isExist(int num) const;
    bool addItem(int num);
    bool removeItem(int num);
    bool isEqual(const CSet& anotherSet);
    CSet intersection(const CSet& anotherSet);
    CSet Union(const CSet& anotherSet);
    // 输出集合中的元素
    void printSet();
    // 析构函数，记得要释放动态申请的内存
    ~CSet() { delete[] intArray; }
private:
    int* intArray;
    // 集合可容纳最多元素个数
    int maxSize;
    // 当前集合大小
    int size;
    // 动态增加集合大小，参数为期望集合大小
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
    {   // 传入的数组长度小于默认的集合大小时
        maxSize = defaultSize;
    }
    else
    {
        maxSize = defaultSize + increasementSize;
    }
    intArray = new int[maxSize];
    // 当前 size 大小为0
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
    {   // 当要加入的元素在集合中不存在时才进行加入
        if(size < maxSize)
        {   // 集合大小足够加入元素时直接进行加入
            intArray[size++] = num;
            return true;
        }
        else
        {   // 集合大小不够时申请内存
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
            cout << "内存申请失败" << endl;
            return false;
        }
        for(int i = 0; i < size; i++)
        {
            temp[i] = intArray[i];
        }
        maxSize = expectedSize + increasementSize;
        // 注意要释放原先申请的内存
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
            {   // 如果存在则将要删除的元素用集合中的最后一个元素覆盖
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
                // 当一个集合中的元素在另一个集合中不存在时则两个集合不同
                return false;
        }
        return true;
    }
    // 集合大小不同，则集合必定不同
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
    // 运行结果如下：
    // s1为：1
    // s2为：1 2 3 4 5 6 7 10
    // s2中是否存在10:1
    // s1和s2的并集：1 2 3 4 5 6 7 10
    // s1中插入1：1
    // s1中插入2：1 2
    // s1和s2的交集：1 2
    // s2移除3、4、5、6、7、10：1 2
    // s1和s2是否相等：1
    
    CSet s1(1);
    cout << "s1为：";
    s1.printSet();

    int nums[10] = {1, 2, 3, 4, 5, 6, 7, 10, 1, 2};
    CSet s2(nums, sizeof(nums) / sizeof(int));
    cout << "s2为：";
    s2.printSet();

    cout << "s2中是否存在10:" << s2.isExist(10) << endl;

    cout << "s1和s2的并集：";
    s1.intersection(s2).printSet();

    s1.addItem(1);
    cout << "s1中插入1：";
    s1.printSet();

    cout << "s1中插入2：";
    s1.addItem(2);
    s1.printSet();

    cout << "s1和s2的交集：";
    s1.Union(s2).printSet();

    cout << "s2移除3、4、5、6、7、10：";
    s2.removeItem(3), s2.removeItem(4), s2.removeItem(5), s2.removeItem(6), s2.removeItem(7), s2.removeItem(10);
    s2.printSet();

    cout << "s1和s2是否相等：" << s1.isEqual(s2) << endl;

    system("pause");
    return 0;
}