#include <iostream>
#include <boost/tokenizer.hpp>

#include "cpu.h"
#include "cartridge.h"
#include "gpu.h"
#include "gbu.h"
#include "gbu_sdl_wrapper.h"
#include "gbu-timer.h"

int main(int argc, char** argv) {

	if (argc == 1)
	{
		std::cout << "No filename give." << std::endl << std::endl;
		std::cout << "usage: " << argv[0] << " filename" << std::endl;
		return -1;
	}

	std::cout << std::setfill('0') << std::setw(4) << std::hex << "Gameboy emu starting up..." << std::endl;

	try
	{
		std::cout << "Reading " << argv[1] << "..." << std::endl;
		Gbu gbu(argv[1], true, false);
		GbuSdlWrapper wrapper(&gbu);
		wrapper.mainloop();
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
}

