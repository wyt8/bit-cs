#include<iostream>
#include "BufferPoolManager.h"
#include<cstdlib>
#include<ctime> 

int main(int argc, char* argv[]) {
    std::cout << "Start Consumer ID = " << argv[0] << std::endl;
    BufferPoolManager bpm;
    srand(time(NULL));
    for(int i = 0; i < 8; i++) {
        Sleep(rand()/10);
        std::string str = bpm.read("Consumer " + std::string(argv[0]));
    }
    return 0;
}