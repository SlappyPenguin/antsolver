#include <iostream>
#include <fstream>
#include <array>
using namespace std;

constexpr size_t X = 55190538;   // <-- set your constant here
constexpr int MAXV = 1000;

int main() {
    ifstream file("turn_clusters.bin", ios::binary);
    if (!file) {
        cerr << "Failed to open file\n";
        return 1;
    }

    // Skip X long longs
    file.seekg(X * sizeof(long long), ios::beg);

    // Frequency array
    array<long long, MAXV> freq{};
    freq.fill(0);

    // Read and count
    for (size_t i = 0; i < X; i++) {
        short value;
        file.read(reinterpret_cast<char*>(&value), sizeof(short));
        if (!file) {
            cerr << "Unexpected EOF\n";
            return 1;
        }

        if (value >= 0 && value < MAXV)
            freq[value]++;
    }

    // Print frequencies
    for (int i = 0; i < MAXV; i++) {
        if (freq[i] > 0)
            cout << i << " " << freq[i] << "\n";
    }

    return 0;
}
