#pragma once
#include <string>

/* AVX2 Support Compile time and Runtime detection */
#if defined(__AVX2__)
constexpr auto hasAVX2ctime = true;
#else
constexpr auto hasAVX2ctime = false;
#endif

#ifdef _MSC_VER
#include <intrin.h>
#else 
#include <cpuid.h>
#endif

static inline int check_avx2() {
#ifdef _MSC_VER
	int cpu_info[4];
	__cpuid(cpu_info, 0);
	if (cpu_info[0] < 7) return 0;

	__cpuidex(cpu_info, 7, 0);
	return (cpu_info[1] & (1 << 5)) != 0; // AVX2 bit
#else
	unsigned int eax, ebx, ecx, edx;
	if (__get_cpuid(0, &eax, &ebx, &ecx, &edx) && eax >= 7) {
		__cpuid_count(7, 0, eax, ebx, ecx, edx);
		return (ebx & (1 << 5)) != 0; // AVX2 bit
	}
	return 0;
#endif
}

#define HAS_AVX2_RUNTIME check_avx2()

#if defined(__cpp_lib_hardware_interference_size)
    #include <new>
    constexpr size_t CACHE_LINE_SIZE = std::hardware_destructive_interference_size;
#elif defined(__GNUC__)
    // Fallback for GCC
    #if defined(__i386__) || defined(__x86_64__)
        constexpr size_t CACHE_LINE_SIZE = 64;
    #elif defined(__powerpc64__)
        constexpr size_t CACHE_LINE_SIZE = 128;
    #elif defined(__arm__) || defined(__aarch64__)
        constexpr size_t CACHE_LINE_SIZE = 64;
    #else
        constexpr size_t CACHE_LINE_SIZE = 64;  // Default fallback
    #endif
#else
    constexpr size_t CACHE_LINE_SIZE = 64;  // Default fallback
#endif


// Cache size detection
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
	#include <cpuid.h>

	inline size_t get_l1_cache_size() {
		unsigned int eax, ebx, ecx, edx;
		if (__get_cpuid(0x80000006, &eax, &ebx, &ecx, &edx)) {
			return (ecx >> 16) & 0xFFFF;  // L1 cache size in KB
		}
		return 32;  // Default 32KB if detection fails
	}
	#define L1_CACHE_SIZE (get_l1_cache_size() * 1024)
#elif defined(_MSC_VER)
	#include <intrin.h>

	inline size_t get_l1_cache_size() {
		int cpu_info[4];
		__cpuid(cpu_info, 0x80000006);
		return (static_cast<size_t>((cpu_info[2] >> 16) & 0xFFFF)) * 1024;  // L1 cache size in bytes
	}
	#define L1_CACHE_SIZE (get_l1_cache_size())
#else
	// Fallback to runtime detection using sysconf if available
	#if defined(_SC_LEVEL1_DCACHE_SIZE)
		#include <unistd.h>
		inline size_t get_l1_cache_size() {
			long size = sysconf(_SC_LEVEL1_DCACHE_SIZE);
			return size > 0 ? size : 32 * 1024;
		}
		#define L1_CACHE_SIZE (get_l1_cache_size())
	#else
		#define L1_CACHE_SIZE (32 * 1024)  // Default fallback
	#endif
#endif


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