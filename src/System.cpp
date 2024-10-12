#include <string>
#include <stdexcept>
#include <sstream>

#if defined(_WIN32)
	#include <windows.h>
	#include <intrin.h>

	// CPUID function numbers
	#define CPUID_BRAND_STRING_START 0x80000002
	#define CPUID_BRAND_STRING_END   0x80000004
	#define CPUID_EXTENDED_FEATURES  0x80000000

	// Size constants
	#define BRAND_STRING_SIZE 0x40
	#define CPUID_DATA_SIZE   4
	#define BRAND_CHUNK_SIZE  16

#elif defined(__APPLE__) || defined(__linux__)
	#include <unistd.h>
	#include <fstream>
	#include <sstream>
	#ifdef __APPLE__
		#include <sys/sysctl.h>
	#endif
#else
	#error "Unsupported Platform"
#endif

#include "System.hpp"
#include "Logger.hpp"

SystemInfo SystemDetector::GetSysInfo() {
	SystemInfo info;
	info.operatingSystem = GetOS();
	info.cpuModel = GetCPUModel();
	info.numCores = GetNumCores();
	info.totalRAM = GetTotalPhysRAM();
	return info;
}

std::string SystemDetector::GetOS() {
#if defined(_WIN32)
	std::ostringstream oss;
	oss << "Windows";

	SYSTEM_INFO sysInfo;
	GetNativeSystemInfo(&sysInfo);
	oss << (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? " 64-bit" : "32-bit");
	return oss.str();

#elif defined(__APPLE__)
	char str[256];
	size_t size = sizeof(str);
	if (sysctlbyname("kern.osrelease", str, &size, NULL, 0) == -1) {
		g_logger->Error("Failed to get OS version for macOS");
		return "macOS";
	}
	
	std::ostringstream oss;
	oss << "macOS " << str;
	return oss.str();

#elif defined(__linux__)
	struct utsname unameData;
	if (uname(&unameData) == -1) {
		g_logger->Error("Failed to get OS information for linux");
		return "linux";
	}
	return std::string(unameData.sysname) + " " + unameData.release;
#endif
}

std::string SystemDetector::GetCPUModel() {
#if defined(_WIN32)
	char brand[BRAND_STRING_SIZE] = { 0 };
	unsigned int cpuInfo[CPUID_DATA_SIZE] = { 0 };
	__cpuid(reinterpret_cast<int*>(cpuInfo), CPUID_EXTENDED_FEATURES);
	unsigned int nExIds = cpuInfo[0];

	if (nExIds < 0x80000004) {
		g_logger->Error("CPUID not supported");
	}

	for (unsigned int i = CPUID_BRAND_STRING_START; i <= CPUID_BRAND_STRING_END; ++i) {
		__cpuid(reinterpret_cast<int*>(cpuInfo), i);
		memcpy(brand + (i - CPUID_BRAND_STRING_START) * BRAND_CHUNK_SIZE, cpuInfo, sizeof(cpuInfo));
	}

	return std::string(brand);

#elif defined(__APPLE__)
	char buffer[1024];
	size_t size = sizeof(buffer);
	if (sysctlbyname("machdep.cpu.brand_string", &buffer, &size, nullptr, 0) < 0) {
		g_logger->Error("Failed to get CPU model");
	}
	return std::string(buffer);

#elif defined(__linux__)
	std::ifstream cpuinfo("/proc/cpuinfo");
	if (!cpuinfo) {
		g_logger->Error("Failed to open /proc/cpuinfo");
	}

	std::string line;
	while (std::getline(cpuinfo, line)) {
		if (line.substr(0, 10) == "model name") {
			std::string::size_type pos = line.find(':');
			if (pos != std::string::npos) {
				return line.substr(pos + 2);
			}
		}
	}
	g_logger->Error("CPU model information not found");
#endif
}

int SystemDetector::GetNumCores() {
#if defined(_WIN32)
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	if (sysInfo.dwNumberOfProcessors == 0) {
		g_logger->Error("Failed to retrieve number of processors");
	}
	return static_cast<int>(sysInfo.dwNumberOfProcessors);

#elif defined(__APPLE__) || defined(__linux__)
	long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
	if (n_procs < 1) {
		g_logger->Error("Failed to retrieve number of processors");
	}
	return static_cast<int>(nprocs);
#endif
}

int64_t SystemDetector::GetTotalPhysRAM() {
#if defined(_WIN32)
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	if (GlobalMemoryStatusEx(&memInfo) == 0) {
		g_logger->Error("Failed to get memory information");
		return -1;
	}
	return static_cast<uint64_t>(memInfo.ullTotalPhys);

#elif defined(__APPLE__)
	int mib[2] = { CTL_HW, HW_MEMSIZE };
	uint64_t ram = 0;
	size_t len = sizeof(ram);
	if (sysctl(mib, 2, &ram, &len, NULL, 0) == -1) {
		g_logger->Error("Failed to get total physical RAM");
		return -1;
	}
	return ram;

#elif defined(__linux__)
	std::ifstream meminfo("/proc/meminfo");
	std::string line;
	while (std::getline(meminfo, line)) {
		if (line.substr(0, 8) == "MemTotal") {
			int64_t ram = std::stoll(line.substr(line.find(":") + 1));
			return ram * 1024;  // Convert from KB to bytes
		}
	}
	g_logger->Error("Could not find MemTotal in /proc/meminfo");
	return 0;
#endif
}