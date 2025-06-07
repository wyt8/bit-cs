#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdio>
#include <iomanip>
#include <sstream>

const int bufferPoolSize = 6;
const int bufferSize = 11;

struct BufferPool
{
    int num = 0;
    char str[bufferPoolSize][bufferSize];
};

class BufferPoolManager
{
public:
    BufferPoolManager()
    {
        this->sharedMemorySize = sizeof(BufferPool);
        this->sharedMemoryKey = "bufferpool";

        this->sharedMemoryHandle = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_EXECUTE_READWRITE, 0, sharedMemorySize, "bufferpool");
        if (this->sharedMemoryHandle == nullptr)
        {
            std::cout << "无法创建共享内存对象：" << GetLastError() << std::endl;
            return;
        }
        this->shareMemoryStart = MapViewOfFile(this->sharedMemoryHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (this->shareMemoryStart == nullptr)
        {
            std::cout << "无法在当前进程创建文件映射：" << GetLastError() << std::endl;
            CloseHandle(this->sharedMemoryHandle);
            return;
        }

        this->emptySemaphore = CreateSemaphoreA(NULL, bufferPoolSize, bufferPoolSize, "empty");
        if (this->emptySemaphore == nullptr)
        {
            std::cout << "empty信号量创建失败：" << GetLastError() << std::endl;
            return;
        }

        this->fullSemaphore = CreateSemaphoreA(NULL, 0, bufferPoolSize, "full");
        if (this->emptySemaphore == nullptr)
        {
            std::cout << "full信号量创建失败：" << GetLastError() << std::endl;
            return;
        }

        this->mutexSemaphore = CreateSemaphoreA(NULL, 1, 1, "mutex");
        if (this->mutexSemaphore == nullptr)
        {
            std::cout << "mutex信号量创建失败：" << GetLastError() << std::endl;
            return;
        }
    }

    void write(std::string str, std::string name)
    {
        if (str.length() > 10)
        { // 防止缓存区溢出
            str = str.substr(0, 10);
        }

        WaitForSingleObject(this->emptySemaphore, INFINITE); // P(empty)
        WaitForSingleObject(this->mutexSemaphore, INFINITE); // P(mutex)

        std::cout << std::endl;
        printInfo("buffer image before");
        struct BufferPool *bp = (struct BufferPool *)(this->shareMemoryStart);
        strcpy_s(bp->str[bp->num], bufferSize, str.c_str());
        bp->num++;
        std::cout << getNowTime() << "\t" << name <<" writes: " << str << std::endl;
        printInfo("buffer image after");
        std::cout << std::endl;

        ReleaseSemaphore(this->mutexSemaphore, 1, nullptr); // V(mutex)
        ReleaseSemaphore(this->fullSemaphore, 1, nullptr); // V(full)
    }

    std::string read(std::string name)
    {
        std::string res;
        WaitForSingleObject(this->fullSemaphore, INFINITE); // P(full)
        WaitForSingleObject(this->mutexSemaphore, INFINITE); // P(mutex)

        std::cout << std::endl;
        printInfo("buffer image before");
        struct BufferPool *bp = (struct BufferPool *)(this->shareMemoryStart);
        bp->num--;
        res = bp->str[bp->num];
        strncpy(bp->str[bp->num], "", 10);
        std::cout << getNowTime() << "\t" << name <<" reads: " << res << std::endl;
        printInfo("buffer image after");
        std::cout << std::endl;

        ReleaseSemaphore(this->mutexSemaphore, 1, nullptr); // V(mutex)
        ReleaseSemaphore(this->emptySemaphore, 1, nullptr); // V(empty)
        return res;
    }

    ~BufferPoolManager()
    {
        // 释放信号量资源
        if (this->emptySemaphore != nullptr)
        {
            CloseHandle(this->emptySemaphore);
        }
        if (this->fullSemaphore != nullptr)
        {
            CloseHandle(this->fullSemaphore);
        }
        if (this->mutexSemaphore != nullptr)
        {
            CloseHandle(this->mutexSemaphore);
        }

        // 释放共享内存资源：先解除文件映射，再关闭共享内存句柄
        if (this->shareMemoryStart != nullptr)
        {
            UnmapViewOfFile(this->shareMemoryStart);
        }
        if (this->sharedMemoryHandle != nullptr)
        {
            CloseHandle(this->sharedMemoryHandle);
        }
    }

private:
    // 共享内存句柄
    HANDLE sharedMemoryHandle;
    // 共享内存大小
    int sharedMemorySize;
    // 共享内存名
    std::string sharedMemoryKey;
    // 共享内存内存段的起始地址
    LPVOID shareMemoryStart;
    // 同步信号量empty
    HANDLE emptySemaphore;
    // 同步信号量full
    HANDLE fullSemaphore;
    // 互斥信号量mutex
    HANDLE mutexSemaphore;

    void printInfo(std::string title)
    { // 打印缓存区映像
        struct BufferPool *bp = (struct BufferPool *)(this->shareMemoryStart);

        // std::string title = "buffer image";
        std::cout << std::setw(20) << std::setfill('-') << "" << title << std::setw(20) << std::setfill('-') << "" << std::endl;
        std::cout << "\t"
                  << "num\t" << bp->num << std::endl;
        std::cout << "\t"
                  << "str[0]\t" << bp->str[0] << std::endl;
        std::cout << "\t"
                  << "str[1]\t" << bp->str[1] << std::endl;
        std::cout << "\t"
                  << "str[2]\t" << bp->str[2] << std::endl;
        std::cout << "\t"
                  << "str[3]\t" << bp->str[3] << std::endl;
        std::cout << "\t"
                  << "str[4]\t" << bp->str[4] << std::endl;
        std::cout << "\t"
                  << "str[5]\t" << bp->str[5] << std::endl;
        std::cout << std::setw(40 + title.length()) << std::setfill('-') << "" << std::endl;
    }

    std::string getNowTime()
    {
        time_t nowtime;
        time(&nowtime);
        tm p;
        localtime_s(&p, &nowtime);
        std::stringstream ss;
        ss << p.tm_year + 1900 << "/" << p.tm_mon + 1 << "/" << p.tm_mday << " " << p.tm_hour << ":" << p.tm_min << ":" << p.tm_sec;
        std::string res;
        ss >> res;
        return ss.str();
    }
};
