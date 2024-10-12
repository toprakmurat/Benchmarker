#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#else
#include <unistd.h>
#include <sys/utsname.h>
#include <fstream>
#endif

#include "System.hpp"

SystemInfo SystemDetector::GetSysInfo() {
	SystemInfo info;
	info.operatingSystem = GetOS();
	info.cpuModel = GetCPUModel();
	info.numCores = GetNumCores();
	info.totalRAM = GetTotalPhysRAM();
	return info;
}

std::string SystemDetector::GetOS() {
#ifdef _WIN32
	return "Windows";
#else
	struct utsname unameData;
	uname(&unameData);
	return std::string(unameData.sysname) + " " + unameData.release;
#endif
}

std::string SystemDetector::GetCPUModel() {
#ifdef _WIN32
	char brand[0x40];
	__cpuid((int*)brand, 0x80000002);
	__cpuid((int*)brand, 0x80000003);
	__cpuid((int*)brand, 0x80000004);
	return brand;

#else
	std::ifstream cpuinfo("/proc/cpuinfo");
	std::string line;
	while (std::getline(cpuinfo, line)) {
		if (line.substr(0, 10) == "model name") {
			return line.substr(line.find(":") + 2);
		}
	}
	return "Unknown";
#endif
}

int SystemDetector::GetNumCores() {
#ifdef _WIN32
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
#else
	return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

int64_t SystemDetector::GetTotalPhysRAM() {
#ifdef _WIN32
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return memInfo.ullTotalPhys;
#else
	std::ifstream meminfo("/proc/meminfo");
	std::string line;
	while (std::getline(meminfo, line)) {
		if (line.substr(0, 8) == "MemTotal") {
			int64_t ram = std::stoll(line.substr(line.find(":") + 1));
			return ram * 1024;  // Convert from KB to bytes
		}
	}
	return 0;
#endif
}