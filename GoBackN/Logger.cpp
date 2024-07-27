#include "Logger.h"

bool Logger::isCreated = false;
std::ofstream Logger::logFile;

void Logger::createLogFile(std::string name) {
	if (Logger::isCreated) {
		return;
	}
	std::string logFileName = "log-";
	// 使用system_clock获取当前时间点
	auto now = std::chrono::system_clock::now();
	// 将时间点转换为时间戳
	auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
	logFileName += name + "-" + std::to_string(timestamp) + ".txt";
	Logger::logFile.open(logFileName);
	if (Logger::logFile.is_open()) {
		Logger::isCreated = true;
		std::cout << "[success] 日志文件创建成功：" << logFileName << std::endl;
	}
	else {
		std::cerr << "[error] 日志文件创建失败" << std::endl;
	}
}

void Logger::closeLogFile() {
	if (Logger::isCreated) {
		Logger::logFile.close();
		Logger::isCreated = false;
		std::cout << "[success] 日志文件关闭成功" << std::endl;
	}
}

void Logger::write(std::string line) {
	if (Logger::isCreated) {
		// 使用system_clock获取当前时间点
		auto now = std::chrono::system_clock::now();
		// 将时间点转换为时间戳
		auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		Logger::logFile << "[" << std::to_string(timestamp) << "] " << line << std::endl;
	}
}
