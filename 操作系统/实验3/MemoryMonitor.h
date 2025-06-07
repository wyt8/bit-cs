#include <iostream>
#include <windows.h>
#include <string>
#include <tlhelp32.h>
#include <psapi.h>

class MemoryMonitor
{
public:
   MemoryMonitor()
   {
      SYSTEM_INFO siSysInfo;
      GetSystemInfo(&siSysInfo);
      pageSize = siSysInfo.dwPageSize;
      allocationGranularity = siSysInfo.dwAllocationGranularity;
      minimumApplicationAddress = siSysInfo.lpMinimumApplicationAddress;
      maximumApplicationAddress = siSysInfo.lpMaximumApplicationAddress;
   }

   void getSystemInfo()
   {
      std::cout << "Page Size:\t" << convertUnit(pageSize, unit::B, unit::KB) << " KB" << std::endl;
      std::cout << "Min App Address:\t" << minimumApplicationAddress << std::endl;
      std::cout << "Max App Address:\t" << maximumApplicationAddress << std::endl;
      std::cout << "Allocation Granularity:\t" << allocationGranularity << std::endl;
   }

   void getGlobalMemoryStatus()
   {
      MEMORYSTATUSEX statex;
      statex.dwLength = sizeof(statex); // 结构体的长度，在使用函数前必须初始化此值
      GlobalMemoryStatusEx(&statex);
      std::cout << "Memory Percentage:\t" << statex.dwMemoryLoad << "%" << std::endl;
      std::cout << "Physical Total Memory Size:\t" << convertUnit(statex.ullTotalPhys, unit::B, unit::GB) << " GB" << std::endl;
      std::cout << "Physical Usable Memory Size:\t" << convertUnit(statex.ullAvailPhys, unit::B, unit::GB) << " GB" << std::endl;
      std::cout << "Virtual Total Memory Size:\t" << convertUnit(statex.ullTotalVirtual, unit::B, unit::TB) << " TB" << std::endl;
      std::cout << "Virtual Usable Memory Size:\t" << convertUnit(statex.ullAvailVirtual, unit::B, unit::TB) << " TB" << std::endl;
      std::cout << "Total PageFile Size:\t" << convertUnit(statex.ullTotalPageFile, unit::B, unit::GB) << " GB" << std::endl;
      std::cout << "Usable PageFile Size:\t" << convertUnit(statex.ullAvailPageFile, unit::B, unit::GB) << " GB" << std::endl;
   }

   // 查询进程虚拟地址空间布局和工作集信息
   void queryProcessMemoryInfo(unsigned pid)
   {
      HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
      if (handle == nullptr)
      {
         std::cout << "Open process failed!\t" << GetLastError() << std::endl;
         return;
      }
      MEMORY_BASIC_INFORMATION mbi;  // 进程虚拟内存空间的基本信息结构
      ZeroMemory(&mbi, sizeof(mbi)); // 分配缓冲区，用于保存信息

      std::cout << "BaseAddress\t"
                << "RegionSize\t"
                << "State\t\t"
                << "Protect\t\t"
                << "Type\t" << std::endl;

      // 循环整个应用程序地址空间
      for (LPCVOID pBlock = minimumApplicationAddress; pBlock < maximumApplicationAddress; pBlock += mbi.RegionSize)
      {
         // VirtualQueryEx检索有关指定进程的虚拟地址空间中的页范围的信息
         if (VirtualQueryEx(handle, pBlock, &mbi, sizeof(mbi)))
         {
            std::cout << mbi.BaseAddress << "\t";
            std::cout << convertUnit(mbi.RegionSize, unit::B, unit::KB) << " KB\t";

            // 显示块的状态
            switch (mbi.State)
            {
            case MEM_COMMIT:
               std::cout << "COMMIT\t\t";
               break;
            case MEM_FREE:
               std::cout << "FREE\t\t";
               break;
            case MEM_RESERVE:
               std::cout << "RESERVE\t\t";
               break;
            }

            // 显示访问保护
            if (mbi.State == MEM_COMMIT)
            {
               switch (mbi.Protect)
               {
               case PAGE_EXECUTE:
                  std::cout << "EXECUTE\t";
                  break;
               case PAGE_EXECUTE_READ:
                  std::cout << "EXECUTE_READ\t";
                  break;
               case PAGE_EXECUTE_READWRITE:
                  std::cout << "EXECUTE_READWRITE\t";
                  break;
               case PAGE_NOACCESS:
                  std::cout << "NOACCESS\t";
                  break;
               case PAGE_READONLY:
                  std::cout << "READONLY\t";
                  break;
               case PAGE_READWRITE:
                  std::cout << "READWRITE\t";
                  break;
               case PAGE_TARGETS_INVALID:
                  std::cout << "TARGETS_INVALID\t";
                  break;
               case PAGE_GUARD:
                  std::cout << "GUARD\t";
                  break;
               case PAGE_NOCACHE:
                  std::cout << "NOCACHE\t";
                  break;
               case PAGE_WRITECOMBINE:
                  std::cout << "WRITECOMBINE\t";
                  break;
               }
            }

            // 显示页面的类型
            if (mbi.State != MEM_FREE)
            {
               switch (mbi.Type)
               {
               case MEM_IMAGE:
                  std::cout << "IMAGE\t";
                  break;
               case MEM_MAPPED:
                  std::cout << "MAPPED\t";
                  break;
               case MEM_PRIVATE:
                  std::cout << "PRIVATE\t";
                  break;
               }
            }
            std::cout << std::endl;
         }
      }
      std::cout << std::endl;
      // 查询工作集相关信息
      PROCESS_MEMORY_COUNTERS pmc;
      ZeroMemory(&pmc, sizeof(pmc));
      GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
      std::cout << "WorkingSet Size:\t" << convertUnit(pmc.WorkingSetSize, unit::B, unit::KB) << " KB" << std::endl;
      std::cout << "Peak WorkingSet Size:\t" << convertUnit(pmc.PeakWorkingSetSize, unit::B, unit::KB) << " KB" << std::endl;
      // 关闭进程句柄
      CloseHandle(handle);
   }

   // 获取系统的存储器使用情况
   void getProformanceInfo()
   {
      PERFORMANCE_INFORMATION pi;
      pi.cb = sizeof(pi);
      GetPerformanceInfo(&pi, sizeof(pi));
      std::cout << "Commit Total:\t" << pi.CommitTotal << std::endl;
      std::cout << "Commit Limit:\t" << pi.CommitLimit << std::endl;
      std::cout << "Commit Peak:\t" << pi.CommitPeak << std::endl;
      std::cout << "Physical Total:\t" << pi.PhysicalTotal << std::endl;
      std::cout << "Physical Available:\t" << pi.PhysicalAvailable << std::endl;
      std::cout << "System Cache:\t" << pi.SystemCache << std::endl;
      std::cout << "Kernel Total:\t" << pi.KernelTotal << std::endl;
      std::cout << "Kernel Paged:\t" << pi.KernelPaged << std::endl;
      std::cout << "Kernel Non Paged:\t" << pi.KernelNonpaged << std::endl;
      std::cout << "Handle Count:\t" << pi.HandleCount << std::endl;
      std::cout << "Process Count:\t" << pi.ProcessCount << std::endl;
      std::cout << "Thread Count:\t" << pi.ThreadCount << std::endl;
   }

   void getRunningProcessInfo()
   {
      HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
      if (hSnapShot == INVALID_HANDLE_VALUE)
      {
         std::cout << "Failed to create snapshot of running processes." << std::endl;
      }

      PROCESSENTRY32 processEntry;
      processEntry.dwSize = sizeof(PROCESSENTRY32);

      if (!Process32First(hSnapShot, &processEntry))
      {
         std::cout << "Failed to read first process entry." << std::endl;
         CloseHandle(hSnapShot);
      }
      std::cout << "PID\t" << "Thread Count\t" << "Parent PID\t" 
         << "Priority\t" << "Name\t"<< std::endl;
      do
      {
         std::cout << processEntry.th32ProcessID << "\t";
         std::cout << processEntry.cntThreads << "\t\t";
         std::cout << processEntry.th32ParentProcessID << "\t\t";
         std::cout << processEntry.pcPriClassBase << "\t\t";
         std::cout << processEntry.szExeFile << std::endl;
      } while (Process32Next(hSnapShot, &processEntry));

      CloseHandle(hSnapShot);
   }

private:
   unsigned pageSize;
   unsigned allocationGranularity;
   void *minimumApplicationAddress;
   void *maximumApplicationAddress;
   enum unit
   {
      B,
      KB,
      MB,
      GB,
      TB
   };

   // 存储单位的转换
   double convertUnit(unsigned long long num, unit before, unit after)
   {
      double res = num;
      if (before < after)
      {
         for (int i = 0; i < after - before; i++)
         {
            res /= 1024.0;
         }
      }
      else
      {
         for (int i = 0; i < before - after; i++)
         {
            res *= 1024.0;
         }
      }
      return res;
   }
};
