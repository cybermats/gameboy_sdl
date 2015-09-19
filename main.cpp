#include <iostream>
#include <boost/tokenizer.hpp>

#include "cpu.h"
#include "cartridge.h"
#include "gpu.h"
#include "gbu.h"
#include "gbu_sdl_wrapper.h"
#include "gbu-timer.h"

using namespace std;

int main() {
    cout << std::setfill('0') << std::setw(4) << hex << "Hello, World!" << endl;

//	const char* filename = "/Users/mats/Documents/Code/gameboyemu/roms/Super Mario Land.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\Super Mario Land.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\cpu_instrs.gb";
	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\individual\\01-special.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\individual\\02-interrupts.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\individual\\03-op sp,hl.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\individual\\04-op r,imm.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\individual\\05-op rp.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\individual\\06-ld r,r.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\individual\\07-jr,jp,call,ret,rst.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\individual\\08-misc instrs.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\individual\\09-op r,r.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\individual\\10-bit ops.gb";
//	const char* filename = "C:\\Users\\mats\\Documents\\gameboy_sdl\\roms\\cpu_instrs\\individual\\11-op a,(hl).gb";



	//#define ENABLE_EXCEPTIONS

#ifdef ENABLE_EXCEPTIONS
	try
	{
#endif
		bool printCallstack = true;
		Gbu gbu(filename, printCallstack);
		GbuSdlWrapper wrapper(&gbu);
		wrapper.mainloop();
#ifdef ENABLE_EXCEPTIONS
	}
	catch (const char* msg)
	{
		std::cout << "Error detected: " << msg << std::endl;
		std::cout << "Aborting." << std::endl;
		return -1;
	}
	catch (const std::string& msg)
	{
		std::cout << "Error detected: " << msg << std::endl;
		std::cout << "Aborting." << std::endl;
		return -1;
	}
	catch (...)
	{
		std::cout << "Unhandled exception." << std::endl;
		std::cout << "Aborting." << std::endl;
		return -1;

	}
#endif


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

