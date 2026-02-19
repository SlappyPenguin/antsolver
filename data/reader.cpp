#include <iostream>
#include <fstream>
using namespace std;

int main() {
    const int X = 50;        // number of lines to read
    const int PER_LINE = 10;

    ifstream file("betstates.bin", ios::binary);
    if (!file) {
        cout << "File not found\n";
        return 1;
    }

    int value;

    for (int i = 0; i < X; ++i) {
        for (int j = 0; j < PER_LINE; ++j) {
            if (!file.read(reinterpret_cast<char*>(&value), sizeof(int))) {
                cout << "Unexpected end of file\n";
                return 1;
            }
            cout << value << " ";
        }
        cout << "\n";
    }

    file.close();
    return 0;
}
