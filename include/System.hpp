#pragma once
#include <string>

struct SystemInfo {
	std::string operatingSystem;
	std::string cpuModel;
	int numCores;
	int64_t totalRAM;  // in bytes
};

class SystemDetector {
public:
	static SystemInfo GetSysInfo();

private:
	static std::string GetOS();
	static std::string GetCPUModel();
	static int GetNumCores();
	static int64_t GetTotalPhysRAM();
};