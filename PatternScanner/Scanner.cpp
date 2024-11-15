#include "Scanner.h"
#include <iomanip>
#include <limits>

Scanner::Scanner(DWORD processId) : processId(processId), hProcess(nullptr), isWow64Process(false) {
    hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (!hProcess) {
        std::cerr << "Failed to open process (PID " << processId << "): " << GetLastError() << std::endl;
        return;
    }

    if (!CheckProcessArchitecture()) {
        std::cerr << "Failed to determine process architecture." << std::endl;
        CloseHandle(hProcess);
        hProcess = nullptr;
    }
}

Scanner::~Scanner() {
    if (hProcess) {
        CloseHandle(hProcess);
    }
}

bool Scanner::IsAddressInDynamicMemory(HANDLE processHandle, uintptr_t address) {
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQueryEx(processHandle, (LPCVOID)address, &mbi, sizeof(mbi)) == 0) {
        std::cerr << "VirtualQueryEx failed. Error: " << GetLastError() << std::endl;
        return false;
    }

    // Check if the region is committed
    if (mbi.State == MEM_COMMIT) {
        // Check if the region is private or shared
        if (mbi.Type == MEM_PRIVATE || mbi.Type == MEM_MAPPED) {
            // Allow more protection types, such as read-only
            if (mbi.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_READONLY | PAGE_EXECUTE_READ)) {
                return true; // Likely dynamic memory
            }
        }
    }

    return false; // Not likely dynamic memory
}

bool Scanner::CheckProcessArchitecture()
{
    BOOL isWow64 = FALSE;
    if (IsWow64Process(hProcess, &isWow64)) {
        isWow64Process = isWow64;
        return true;
    }
    std::cerr << "Failed to determine if process is Wow64: " << GetLastError() << std::endl;
    return false;
}

bool Scanner::Parse(const std::string& pattern, std::vector<BYTE>& byte, std::string& mask) {
    byte.clear();
    mask.clear();

    size_t len = pattern.length();
    for (size_t i = 0; i < len; ++i) {
        if (pattern[i] == '?') {
            // Check for the "??" wildcard
            if (i + 1 < len && pattern[i + 1] == '?') {
                mask += '?';
                byte.push_back(0x00); // Placeholder for wildcard
                i++; // Skip the next '?' character
            }
            else {
                return false; // If there's only one '?', it's an invalid pattern
            }
        }
        else if (std::isxdigit(pattern[i]) && i + 1 < len && std::isxdigit(pattern[i + 1])) {
            // Convert the next two hex characters to a byte
            mask += 'x';
            byte.push_back(static_cast<BYTE>(std::stoi(pattern.substr(i, 2), nullptr, 16)));
            i++; // Skip the next character as it's part of this byte
        }
        else if (pattern[i] == ' ') {
            // Skip spaces
            continue;
        }
        else {
            // Invalid character found in the pattern
            return false;
        }
    }

    // Debug output for the parsed pattern and mask
#ifdef DEBUG
    std::cout << "Parsed pattern bytes: ";
#endif

#ifdef DEBUG
    for (BYTE b : byte) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
    }
#endif

#ifdef DEBUG
    std::cout << "\nParsed mask: " << mask << std::endl;
#endif

    return true;
}

uintptr_t Scanner::Scan(const std::string& pattern, bool onlyDynamicMemory) {
    if (!hProcess) {
        std::cerr << "Process handle is invalid." << std::endl;
        return 0;
    }

    std::vector<BYTE> bytePattern;
    std::string mask;
    if (!Parse(pattern, bytePattern, mask)) {
        std::cerr << "Invalid pattern format!" << std::endl;
        return 0;
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    uintptr_t startAddress = (uintptr_t)sysInfo.lpMinimumApplicationAddress;
    uintptr_t endAddress = (uintptr_t)sysInfo.lpMaximumApplicationAddress;

    MEMORY_BASIC_INFORMATION mbi;
    uintptr_t currentAddress = startAddress;

    while (currentAddress < endAddress) {
        if (VirtualQueryEx(hProcess, (LPCVOID)currentAddress, &mbi, sizeof(mbi))) {
#ifdef DEBUG
            std::cout << "Scanning region at: " << std::hex << currentAddress
                << " of size: " << mbi.RegionSize << std::endl;
#endif

            if (mbi.State == MEM_COMMIT &&
                (mbi.Protect & (PAGE_READWRITE | PAGE_READONLY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE))) {

                // Check if the memory region is dynamic if onlyDynamicMemory is true
                if (onlyDynamicMemory && !IsAddressInDynamicMemory(hProcess, (uintptr_t)mbi.BaseAddress)) {
                    currentAddress += mbi.RegionSize; 
                    continue;
                }

                uintptr_t foundAddress = Scan((uintptr_t)mbi.BaseAddress, mbi.RegionSize, bytePattern, mask);
                if (foundAddress) {
                    return foundAddress;
                }
            }
            currentAddress += mbi.RegionSize;
        }
        else {
            break;
        }
    }
    return 0;
}

uintptr_t Scanner::Scan(uintptr_t startAddress, size_t size, const std::vector<BYTE>& pattern, const std::string& mask) {
    std::vector<BYTE> buffer(size);
    if (!ReadProcessMemory(hProcess, (LPCVOID)startAddress, buffer.data(), size, nullptr))
    {
#ifdef DEBUG
        std::cerr << "Failed to read memory at address: " << std::hex << startAddress
            << " Error: " << GetLastError() << std::endl;
#endif
        return 0;
    }

    size_t patternLength = pattern.size();
    for (size_t i = 0; i <= size - patternLength; ++i) {
        bool found = true;
        for (size_t j = 0; j < patternLength; ++j) {
            if (mask[j] == 'x' && buffer[i + j] != pattern[j]) {
                found = false;
                break;
            }
        }
        if (found)
        {
#ifdef DEBUG
            std::cout << "Pattern found at offset: " << std::hex << i << std::endl;
#endif
            return startAddress + i;
        }
    }
    return 0;
}
