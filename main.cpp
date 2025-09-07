#include "emulator/emulator.h"
using namespace std;
int main() {
    string filename;
    cout << "Nhap duong dan file ASM: ";
    getline(cin, filename);

    runEmulator(filename);
    return 0;
}
