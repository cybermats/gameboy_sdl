#include <iostream>
#include <boost/tokenizer.hpp>

#include "cpu.h"
#include "cartridge.h"
#include "gpu.h"
#include "gbu.h"
#include "gbu_sdl_wrapper.h"

using namespace std;

int main() {
    cout << std::setfill('0') << std::setw(4) << hex << "Hello, World!" << endl;

    const char* filename = "/Users/mats/Documents/Code/gameboyemu/roms/Super Mario Land.gb";

    Gbu gbu(filename);
    GbuSdlWrapper wrapper(&gbu);
    wrapper.mainloop();


/*
    std::cout << "Creating cartridge" << std::endl;
    Cartridge cartridge(filename);
    std::cout << "Creating MBC" << std::endl;
    auto mbc = cartridge.getMBC();
    std::cout << "Creating MMU" << std::endl;
    MMU mmu(mbc.get());
    Gpu gpu(&mmu);
    mmu.setGpu(&gpu);

    std::cout << "Creating CPU" << std::endl;
    Cpu cpu(&mmu);


    std::cout << "Running:" << std::endl << std::endl;

    cpu.printStep();
    char c;

    for(;;)
    {
        std::cin >> c;

        switch (c)
        {
            case 'q':
                std::cout << "Quitting." << std::endl;
                return 0;
            case 'c':
            {
                int p = std::cin.peek();
                if(p == EOF || p == '\n')
                    p = 1;
                else
                    std::cin >> std::hex >> p;
                for(int i = 0; i < p; ++i)
                {
                    cpu.execute();
                    gpu.checkline(cpu.m);
                    cpu.printStep();
                }
                break;
            }
            case 'l':
                cpu.printCode(10);
                break;
            case 's':
                cpu.printStep();
                break;
            case 'g':
                int goal;
                std::cin >> std::hex >> goal;
                std::cout << "Go to: " << std::hex << goal << std::endl;
                while(cpu.getPc() != goal)
                {
                    cpu.execute();
                    gpu.checkline(cpu.m);
//                    cpu.printStep();
                }
                std::cout << "Finished." << std::endl;
                break;

            case 'h':
            case '?':
                std::cout << "Commands available:" << std::endl;
                std::cout << "q - quit." << std::endl;
                std::cout << "c - step" << std::endl;
                std::cout << "l - list next ten codes" << std::endl;
                std::cout << "s - show state" << std::endl;
                std::cout << "h - help" << std::endl;
                break;
        }
    }
*/
}

