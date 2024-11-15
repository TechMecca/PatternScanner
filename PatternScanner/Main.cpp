#define NOMINMAX 
#include "Scanner.h"
#include <iostream>
#include <string>
#include <limits>

int main() {
    DWORD processId;
    char continueChoice;

    // Prompt for Process ID
    std::cout << "Enter the target process ID: ";
    std::cin >> processId;

    // Clear the input buffer to handle the newline character left by std::cin
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Create the Scanner object with the given process ID
    Scanner scanner(processId);

    do {
        std::string pattern;
        bool onlyDynamicMemory;

        std::cout << "\n------------------------------------------------" << std::endl;
        // Prompt for Pattern
        std::cout << "Enter the pattern (e.g., 48 8B ?? ?? ?? ?? ?? 89 ?? ??): ";
        std::getline(std::cin, pattern);

        // Prompt to search only in dynamic memory
        char dynamicChoice;
        std::cout << "Search only in dynamic memory? (y/n): ";
        std::cin >> dynamicChoice;
        onlyDynamicMemory = (dynamicChoice == 'y' || dynamicChoice == 'Y');

        // Perform the scan with the provided pattern and dynamic memory option
        uintptr_t foundAddress = scanner.Scan(pattern, onlyDynamicMemory);

        // Output the result
        if (foundAddress) {
            std::cout << "Pattern found at address: 0x" << std::hex << foundAddress << std::endl;
        }
        else {
            std::cout << "Pattern not found." << std::endl;
        }

        // Ask if the user wants to scan for another pattern
        std::cout << "Do you want to scan for another pattern? (y/n): ";
        std::cin >> continueChoice;

        // Clear the input buffer for the next loop iteration
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "------------------------------------------------" << std::endl;

    } while (continueChoice == 'y' || continueChoice == 'Y');

    std::cout << "Exiting the scanner. Goodbye!" << std::endl;
    return 0;
}


