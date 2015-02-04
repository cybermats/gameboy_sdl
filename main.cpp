#include <iostream>

#include "cpu.h"
#include "cartridge.h"
#include "mbc.h"
#include "mmu.h"

using namespace std;

int main() {
    cout << hex << "Hello, World!" << endl;

    const char* filename = "/Users/mats/Documents/Code/gameboyemu/roms/Super Mario Land.gb";

    std::cout << "Creating cartridge" << std::endl;
    Cartridge cartridge(filename);
    std::cout << "Creating MBC" << std::endl;
    auto mbc = cartridge.getMBC();
    std::cout << "Creating MMU" << std::endl;
    MMU mmu(mbc.get());
    std::cout << "Creating CPU" << std::endl;
    Cpu cpu(&mmu);

    std::cout << "Running:" << std::endl << std::endl;

    char c;

    for(int i = 0; i < 10; ++i)
    {
        cpu.printRegisters();
        cpu.printCode(1);
        std::cin >> c;
        if(c == 'q')
            break;
        cpu.execute();
    }




    return 0;
}

