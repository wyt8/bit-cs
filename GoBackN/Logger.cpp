#include "Logger.h"

bool Logger::isCreated = false;
std::ofstream Logger::logFile;

void Logger::createLogFile(std::string name) {
	if (Logger::isCreated) {
		return;
	}
	std::string logFileName = "log-";
	// ʹ��system_clock��ȡ��ǰʱ���
	auto now = std::chrono::system_clock::now();
	// ��ʱ���ת��Ϊʱ���
	auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
	logFileName += name + "-" + std::to_string(timestamp) + ".txt";
	Logger::logFile.open(logFileName);
	if (Logger::logFile.is_open()) {
		Logger::isCreated = true;
		std::cout << "[success] ��־�ļ������ɹ���" << logFileName << std::endl;
	}
	else {
		std::cerr << "[error] ��־�ļ�����ʧ��" << std::endl;
	}
}

void Logger::closeLogFile() {
	if (Logger::isCreated) {
		Logger::logFile.close();
		Logger::isCreated = false;
		std::cout << "[success] ��־�ļ��رճɹ�" << std::endl;
	}
}

void Logger::write(std::string line) {
	if (Logger::isCreated) {
		// ʹ��system_clock��ȡ��ǰʱ���
		auto now = std::chrono::system_clock::now();
		// ��ʱ���ת��Ϊʱ���
		auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		Logger::logFile << "[" << std::to_string(timestamp) << "] " << line << std::endl;
	}
}
