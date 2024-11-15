#ifndef SCANNER_H
#define SCANNER_H

#include <iostream>
#include <vector>
#include <windows.h>
#include <string>

class Scanner {
public:
    explicit Scanner(DWORD processId);
    ~Scanner();

    uintptr_t Scan(const std::string& pattern, bool onlyDynamicMemory);

private:
    DWORD processId;
    HANDLE hProcess;
    bool isWow64Process; // Indicates if the target process is a Wow64 (32-bit on 64-bit OS) process

    bool CheckProcessArchitecture();
    bool Parse(const std::string& pattern, std::vector<BYTE>& byte, std::string& mask);
    uintptr_t Scan(uintptr_t startAddress, size_t size, const std::vector<BYTE>& pattern, const std::string& mask);
    bool IsAddressInDynamicMemory(HANDLE processHandle, uintptr_t address);
};

#endif // SCANNER_H
