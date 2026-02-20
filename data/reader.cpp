#include <iostream>
#include <fstream>
#include <vector>

int main() {
    std::size_t X, Y;
    X = 100;
    Y = 5;

    std::ifstream file("blueprint.bin", std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open a.bin\n";
        return 1;
    }

    std::vector<float> buffer(X * Y);

    file.read(reinterpret_cast<char*>(buffer.data()),
              buffer.size() * sizeof(float));

    std::size_t read_count = file.gcount() / sizeof(float);

    for (std::size_t i = 0; i < read_count; ++i) {
        std::cout << buffer[i] << " ";
        if ((i + 1) % Y == 0)
            std::cout << "\n";
    }

    // If file ended mid-line
    if (read_count % Y != 0)
        std::cout << "\n";

    return 0;
}