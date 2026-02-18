#include <iostream>
#include <fstream>
using namespace std;

int main() {
    int X = 100000;

    ifstream file("gamestates.bin", ios::binary);
    if (!file) {
        cerr << "Error opening file\n";
        return 1;
    }

    int freq[3] = {};

    int buffer[9];
    for (int i = 0; i < X; i++) {
        file.read(reinterpret_cast<char*>(buffer), sizeof(buffer));

        if (!file) {
            cerr << "Error reading block " << i << "\n";
            return 1;
        }

        // for (int j = 0; j < 9; j++) {
        //     cout << buffer[j];
        //     if (j < 8) cout << " ";
        // }
        // cout << "\n";

        freq[buffer[0]]++;
    }

    cout << freq[0] << " " << freq[1] << " " << freq[2] << endl;

    file.close();
    return 0;
}
