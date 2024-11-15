#define NOMINMAX 
#include "Scanner.h"
#include <iostream>
#include <string>
#include <limits>

#include "Scanner.h"
#include <iostream>
#include <string>
#include <limits>

int main() {
    DWORD processId;
    std::string pattern;
    bool onlyDynamicMemory;

    // Prompt for Process ID
    std::cout << "Enter the target process ID: ";
    std::cin >> processId;

    // Clear the input buffer to handle the newline character left by std::cin
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Prompt for Pattern
    std::cout << "Enter the pattern (e.g., 48 8B ?? ?? ?? ?? ?? 89 ?? ??): ";
    std::getline(std::cin, pattern);

    // Prompt to search only in dynamic memory
    char dynamicChoice;
    std::cout << "Search only in dynamic memory? (y/n): ";
    std::cin >> dynamicChoice;
    onlyDynamicMemory = (dynamicChoice == 'y' || dynamicChoice == 'Y');
    // Create the Scanner object with the given process ID
    Scanner scanner(processId);

    // Perform the scan with the provided pattern and dynamic memory option
    uintptr_t foundAddress = scanner.Scan(pattern, onlyDynamicMemory);

    // Output the result
    if (foundAddress) {
        std::cout << "Pattern found at address: 0x" << std::hex << foundAddress << std::endl;
    }
    else {
        std::cout << "Pattern not found." << std::endl;
    }

    return 0;
}

