#include <iostream>
#include <fstream>

int main() {
    const char* filename = "river_sets.bin";  // change as needed

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    std::streamsize size = file.tellg();
    file.close();

    if (size % sizeof(long long) != 0) {
        std::cerr << "Warning: File size is not a multiple of sizeof(long long)\n";
    }

    std::cout << "Number of long long values: "
              << size / sizeof(long long)
              << "\n";

    return 0;
}
