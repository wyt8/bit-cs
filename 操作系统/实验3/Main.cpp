#include <iostream>
#include <iomanip>
#include <string>
#include "MemoryMonitor.h"

// 能够实时显示当前系统中内存的使用情况，包括系统地址空间的布局，物理内存的使用情况，系统正在运行的进程信息
// 能够实时显示某个进程的虚拟地址空间布局和工作集信息等

int main()
{
    MemoryMonitor m;
    int index = 0;

    while (true && index != 4)
    {
        std::cout << std::setw(20) << std::setfill('-') << ""
                  << "MemoryManger" << std::setw(20) << std::setfill('-') << "" << std::endl;
        std::cout << "1\t"
                  << "Memory Status" << std::endl;
        std::cout << "2\t"
                  << "Running Processes Info" << std::endl;
        std::cout << "3\t"
                  << "Virtual Address Space Layout and WorkingSet Information of a Process" << std::endl;
        std::cout << "4\t"
                  << "Exit" << std::endl;
        std::cout << std::setw(52) << std::setfill('-') << "" << std::endl;
        std::cout << std::endl
                  << "Please Input the Index of Instruction: \t";
        std::cin >> index;
        std::cout << std::endl;
        switch (index)
        {
        case 1:
            m.getSystemInfo();
            m.getGlobalMemoryStatus();
            m.getProformanceInfo();
            break;
        case 2:
            m.getRunningProcessInfo();
            break;
        case 3:
            unsigned pid;
            std::cout << "Please Input PID: \t";
            std::cin >> pid;
            m.queryProcessMemoryInfo(pid);
            break;
        case 4:
            std::cout << "Bye!" << std::endl;
            break;
        default:
            std::cout << "Wrong Command!" << std::endl;
        }
        std::cout << std::endl;
    }
    system("pause");
    return 0;
}