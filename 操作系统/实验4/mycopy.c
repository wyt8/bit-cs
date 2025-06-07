#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string.h>
using namespace std;

bool mycopy(string sourcePath, string destPath)
{
struct stat s;
cout << sourcePath << endl;
cout << destPath << endl;
if (lstat(sourcePath.c_str(), &s) == -1)
{
    perror("stat error!");
    return false;
}

    // 判断源文件是否为目录
    if (S_ISDIR(s.st_mode))
    {
        DIR *dirp = opendir(sourcePath.c_str());
        if (dirp == nullptr)
        {
            perror("opendir error!");
            return false;
        }

        mkdir(destPath.c_str(), s.st_mode);
        struct dirent *direntp;
        while ((direntp = readdir(dirp)) != nullptr)
        {
            string tempSourcePath = sourcePath;
            string tempDestPath = destPath;
            if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
                continue;
            bool res = mycopy(tempSourcePath.append("/").append(direntp->d_name).c_str(), tempDestPath.append("/").append(direntp->d_name).c_str());
            if (res == false)
                return false;
        }
        closedir(dirp);
        return true;
    }
    // 普通文件
    else if (S_ISREG(s.st_mode))
    {
        int sourceFileHandle, destFileHandle;
        sourceFileHandle = open(sourcePath.c_str(), O_RDONLY);
        destFileHandle = open(destPath.c_str(), O_WRONLY | O_CREAT, s.st_mode);
        if (sourceFileHandle == -1 || destFileHandle == -1)
        {
            perror("open error!");
            return false;
        }

        // 缓存区
        char buf[1024] = {0};
        int len;
        while ((len = read(sourceFileHandle, buf, 1024)) != 0)
        {
            write(destFileHandle, buf, len);
        }
        close(sourceFileHandle);
        close(destFileHandle);
        return true;
    }
    // 链接文件
    else if (S_ISLNK(s.st_mode))
    {
        char buf[1024] = {0};
        int len;
        len = readlink(sourcePath.c_str(), buf, sizeof(buf));
        if (len == -1)
        {
            perror("readlink error");
            return false;
        }
        if(symlink(buf, destPath.c_str()) == -1)
        {
            perror("symlink error!");
            return false;
        }
        return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        perror("lack of params!");
        return -1;
    }
    char *sourcePath = argv[1];
    char *destPath = argv[2];
    if (mycopy(string(sourcePath), string(destPath)))
    {
        cout << "copy success!" << endl;
        return 0;
    }
    else
    {
        cout << "copy fail!" << endl;
        return -1;
    }
}