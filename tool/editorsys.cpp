#define _WIN32_WINNT 0x0601//vista\win7
#include <Windows.h>
#include <intrin.h>
#include <iostream>
#include <string>
#include <vector>

// ==================== ϵͳ��Ϣ�ṹ�� ====================
struct SystemInfoEx {
    std::string windowsVersion;
    std::string cpuName;
    unsigned int logicalCores;
    unsigned int physicalCores;
    bool is64BitSystem;
    unsigned long long totalMemoryMB;
    int l1CacheKB;
    int l2CacheKB;
    int l3CacheKB;
};

// ==================== ��ȡ Windows �汾 ====================
typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

std::string GetWindowsVersion() {
    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
    if (!hMod) return "Unknown";
    auto pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
    if (!pRtlGetVersion) return "Unknown";

    RTL_OSVERSIONINFOW rovi = { 0 };
    rovi.dwOSVersionInfoSize = sizeof(rovi);
    if (pRtlGetVersion(&rovi) == 0) {
        char buf[128];
        sprintf_s(buf, "Windows %lu.%lu (Build %lu)",
            rovi.dwMajorVersion, rovi.dwMinorVersion, rovi.dwBuildNumber);
        return buf;
    }
    return "Unknown";
}

// ==================== ��ȡ CPU ���� ====================
std::string GetCpuName() {
    int regs[4] = { 0 };
    char brand[0x40] = { 0 };

    __cpuid(regs, 0x80000000);
    unsigned int maxExtIds = regs[0];

    if (maxExtIds >= 0x80000004) {
        __cpuid((int*)regs, 0x80000002);
        memcpy(brand, regs, sizeof(regs));
        __cpuid((int*)regs, 0x80000003);
        memcpy(brand + 16, regs, sizeof(regs));
        __cpuid((int*)regs, 0x80000004);
        memcpy(brand + 32, regs, sizeof(regs));
    }
    return std::string(brand);
}

// ==================== ��ȡ�߼�����&������� ====================
void GetCoreInfo(unsigned int& logical, unsigned int& physical) {
    logical = physical = 0;
    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &len);
    std::vector<BYTE> buffer(len);
    if (GetLogicalProcessorInformationEx(RelationProcessorCore,
        (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer.data(), &len)) {

        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ptr =
            (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer.data();

        while ((BYTE*)ptr < buffer.data() + len) {
            if (ptr->Relationship == RelationProcessorCore)
                physical++;
            ptr = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)
                ((BYTE*)ptr + ptr->Size);
        }
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    logical = sysInfo.dwNumberOfProcessors;
}

// ==================== �ж�ϵͳ�Ƿ� 64 λ ====================
bool IsSystem64Bit() {
    BOOL isWow64 = FALSE;
    BOOL ok = IsWow64Process(GetCurrentProcess(), &isWow64);
    if (!ok) return false;
    return isWow64; // true ��ʾ��������32λʱ��������64λϵͳ
}

// ==================== ��ȡ�ڴ������� ====================
unsigned long long GetTotalMemoryMB() {
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem))
        return mem.ullTotalPhys / (1024 * 1024);
    return 0;
}

// ==================== ��ȡ CPU �����С��L1/L2/L3�� ====================
void GetCpuCacheInfo(int& l1, int& l2, int& l3) {
    l1 = l2 = l3 = 0;
    int regs[4];
    for (int i = 0;; ++i) {
        __cpuidex(regs, 4, i);
        int cacheType = regs[0] & 0x1F;
        if (cacheType == 0) break; // û�и��� cache

        int level = (regs[0] >> 5) & 0x7;
        int lineSize = (regs[1] & 0xFFF) + 1;
        int partitions = ((regs[1] >> 12) & 0x3FF) + 1;
        int ways = ((regs[1] >> 22) & 0x3FF) + 1;
        int sets = regs[2] + 1;
        int cacheSize = (ways * partitions * lineSize * sets) / 1024; // KB

        if (level == 1) l1 += cacheSize;
        else if (level == 2) l2 += cacheSize;
        else if (level == 3) l3 += cacheSize;
    }
}

// ==================== ���߼� ====================
int main() {
    SystemInfoEx info;

    info.windowsVersion = GetWindowsVersion();
    info.cpuName = GetCpuName();
    GetCoreInfo(info.logicalCores, info.physicalCores);
    info.is64BitSystem = IsSystem64Bit();
    info.totalMemoryMB = GetTotalMemoryMB();
    GetCpuCacheInfo(info.l1CacheKB, info.l2CacheKB, info.l3CacheKB);

    std::cout << "==== ϵͳ��Ϣ ====\n";
    std::cout << "Windows �汾   : " << info.windowsVersion << "\n";
    std::cout << "CPU ����       : " << info.cpuName << "\n";
    std::cout << "�߼�������     : " << info.logicalCores << "\n";
    std::cout << "���������     : " << info.physicalCores << "\n";
    std::cout << "�Ƿ�64λϵͳ   : " << (info.is64BitSystem ? "��" : "��") << "\n";
    std::cout << "�ڴ��ܴ�С(MB) : " << info.totalMemoryMB << "\n";
    std::cout << "L1 Cache (KB)  : " << info.l1CacheKB << "\n";
    std::cout << "L2 Cache (KB)  : " << info.l2CacheKB << "\n";
    std::cout << "L3 Cache (KB)  : " << info.l3CacheKB << "\n";

    return 0;
}
