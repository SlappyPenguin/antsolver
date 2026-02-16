#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// ======== Constants ========
constexpr const char* FILE_NAME = "turn_distributions.bin";
constexpr size_t X = 55190538;  // number of long longs to skip
constexpr size_t Y = 500;   // number of floats to read
constexpr size_t Z = 10;   // floats per line
// ============================

int main() {
    ifstream file(FILE_NAME, ios::binary);
    if (!file) {
        cerr << "Failed to open file\n";
        return 1;
    }

    // ---- Skip X long longs ----
    file.seekg(X * sizeof(long long), ios::cur);
    if (!file) {
        cerr << "Failed while skipping long longs\n";
        return 1;
    }

    // ---- Read Y floats ----
    vector<float> values(Y);
    file.read(reinterpret_cast<char*>(values.data()), Y * sizeof(float));
    if (!file) {
        cerr << "Failed reading floats\n";
        return 1;
    }

    // ---- Print floats Z per line ----
    for (size_t i = 0; i < values.size(); ++i) {
        cout << values[i];
        if ((i + 1) % Z == 0)
            cout << '\n';
        else
            cout << ' ';
    }

    // Add newline if last line wasn't full
    if (values.size() % Z != 0)
        cout << '\n';

    return 0;
}
