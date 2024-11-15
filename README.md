---

# Memory Pattern Scanner

A versatile tool to scan the memory of a target process for specific byte patterns. Ideal for reverse engineering, debugging, and memory analysis, the scanner supports wildcard matching and dynamic memory filtering.

## Features

- **Cross-Architecture Support**: Works with both x86 (32-bit) and x64 (64-bit) processes.
- **Dynamic Memory Filtering**: Option to scan only dynamically allocated memory regions (e.g., heap allocations).
- **Wildcard Support**: Use `??` in patterns to represent unknown or variable bytes.
- **Debug Logging**: Enable detailed logs for memory scanning and troubleshooting.

---

## Getting Started

### Prerequisites

- **Windows Operating System** (required for Windows-specific APIs like `VirtualQueryEx` and `ReadProcessMemory`).
- **Administrator Privileges** (to access protected processes).

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/memory-pattern-scanner.git
   cd memory-pattern-scanner
   ```

2. Compile the program:
   - **Visual Studio**:
     - Open the project in Visual Studio.
     - Build it in `Debug` or `Release` mode.
   - **g++** (for command line):
     ```bash
     g++ -o PatternScanner.exe main.cpp Scanner.cpp -std=c++17
     ```

---

## Usage

Run the program and follow the prompts:

1. **Enter Target Process ID**:
   - Input the PID of the target process. You can find this in Task Manager or tools like Process Explorer.
2. **Enter Pattern**:
   - Provide the pattern to search for in hexadecimal format, separated by spaces. Use `??` for wildcards.
   - Example: `48 8B ?? ?? ?? ?? ?? 89 ?? ??`
3. **Search Only in Dynamic Memory**:
   - Type `y` to restrict the search to dynamically allocated memory (e.g., heap regions).
   - Type `n` to scan all accessible memory regions.

### Example Input
```plaintext
Enter the target process ID: 1234
Enter the pattern (e.g., 48 8B ?? ?? ?? ?? ?? 89 ?? ??): 48 8B ?? 89 50 ?? ?? 90
Search only in dynamic memory? (y/n): y
```

### Example Output
```plaintext
Pattern found at address: 0x7ff68742a120
```

---

## Debugging

Enable debug logging during compilation by defining the `DEBUG` preprocessor flag:
- **Visual Studio**:
  - Add `DEBUG` to **Preprocessor Definitions** in the project settings.
- **g++**:
  ```bash
  g++ -o PatternScanner.exe main.cpp Scanner.cpp -std=c++17 -DDEBUG
  ```

With debugging enabled, logs will include:
- Memory regions being scanned.
- Details of the parsed pattern and mask.
- Errors encountered during memory access.

---

## Limitations

1. Requires administrator privileges to access certain processes.
2. Cross-architecture scanning (e.g., scanning a 32-bit process from a 64-bit scanner) may require additional handling.
3. The scanner returns only the first match. Extending it for multiple matches is possible but currently unsupported.

---

## Contributing

Contributions are welcome! If you encounter bugs, have feature suggestions, or want to improve the code, feel free to open an issue or submit a pull request.



## Acknowledgments

Special thanks to the Windows API documentation and community tools like Cheat Engine and Process Explorer, which inspired this project's development.

---

Feel free to customize the `git clone` link or add more details if needed! This README provides a comprehensive overview and instructions for users and contributors.
