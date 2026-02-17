#include <iostream>
#include <fstream>
#include <array>

int main() {
    const std::string filename = "river_clusters.bin";

    constexpr size_t X = 2428287420;   // number of long longs to skip
    constexpr int MAXV = 1000;      // short range [0, 999]

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    // Skip X long longs
    std::streamoff offset =
        static_cast<std::streamoff>(X) * sizeof(long long);

    file.seekg(offset, std::ios::beg);

    if (!file) {
        std::cerr << "Seek failed\n";
        return 1;
    }

    std::array<unsigned long long, MAXV> freq{};
    short value;

    // Read X shorts and count
    for (size_t i = 0; i < X; ++i) {
        file.read(reinterpret_cast<char*>(&value), sizeof(short));
        if (!file) {
            std::cerr << "Error reading short at index " << i << "\n";
            return 1;
        }

        if (value < 0 || value >= MAXV) {
            std::cerr << "Short out of range at index " << i << "\n";
            return 1;
        }

        ++freq[value];
    }

    // Print frequencies
    for (int i = 0; i < MAXV; ++i) {
        std::cout << i << " : " << freq[i] << "\n";
    }

    return 0;
}
