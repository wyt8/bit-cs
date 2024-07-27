#include "Config.h"

std::string Config::configFilePath = "./config.conf";

/* 默认配置如下 */

unsigned short Config::UDPPort = 40964;
unsigned short Config::DataSize = 1024;
int Config::ErrorRate = 0;
int Config::LostRate = 0;
int Config::SWSize = 7;
int Config::InitSeqNo = 1;
int Config::Timeout = 1000;

void Config::readConfigFile() {
	std::ifstream file;
	file.open(Config::configFilePath, std::ios::in);
	if (!file.is_open()) {
		std::cerr << "[warning] 无法打开配置文件，使用默认配置" << std::endl;
		return;
	}

	std::string line;
	while (getline(file, line)) { // 逐行读取文件内容
		auto index = line.find('=');
		std::string key = line.substr(0, index);
		int value = std::stoi(line.substr(index + 1));
		if (key == "UDPPort")
			Config::UDPPort = value;
		else if (key == "DataSize")
			Config::DataSize = value;
		else if (key == "ErrorRate")
			Config::ErrorRate = value;
		else if (key == "LostRate")
			Config::LostRate = value;
		else if (key == "SWSize")
			Config::SWSize = value;
		else if (key == "InitSeqNo")
			Config::InitSeqNo = value;
		else if (key == "Timeout")
			Config::Timeout = value;
	}
	std::cout << "[success] 读取配置文件成功" << std::endl;
	file.close(); // 关闭文件
	return;
}
