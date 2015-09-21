#include <fstream>
#include <iostream>
#include <string>

#include "gb_exception.h"
#include "cartridge.h"

Cartridge::Cartridge(const char *filename)
    : _filename(filename)
{}


std::unique_ptr<IMBC> Cartridge::getMBC() {
    std::cout << "getMBC" << std::endl;
	std::vector<unsigned char> buffer = readFile(_filename);
	_title = getTitle(buffer);
    unsigned char mbcType = getCartridgeType(buffer);
    size_t ramSize = getRamSize(buffer);

	std::cout << "Title: " << _title << std::endl;
	std::cout << "cartridge type:" << (int)mbcType << std::endl;
	std::cout << "Ram size:" << ramSize << std::endl;

	// Fix things
	if (_title == "SUPER MARIOLAND")
		ramSize = 0x2000;


    switch (mbcType)
    {
	case 0x00:
		return std::unique_ptr<IMBC>(new ROM(buffer));
    case 0x01:
    case 0x02:
    case 0x03:
		return std::unique_ptr<IMBC>(new MBC1(buffer, std::vector<unsigned char>(ramSize, 0)));
    default:
		throw gb_exception("Unknown cartridge type.");
    }
}



unsigned char Cartridge::getCartridgeType(const std::vector<unsigned char>& buffer)
{
    return buffer[0x147];
}

size_t Cartridge::getRamSize(const std::vector<unsigned char>& buffer)
{
    switch (buffer[0x149])
    {
        case 0:
			return 0;
        case 1:
            return 2048;
        case 2:
            return 8192;
        case 3:
            return 32768;
        default:
            return 0;
    }
}

std::vector<unsigned char> Cartridge::readFile(const char* filename)
{
    std::ifstream file(filename, std::ios::binary|std::ios::ate);
	if (!file.good())
		throw "Unable to read file: " + std::string(filename);
    auto size = file.tellg();
    std::vector<unsigned char> buffer(size);
    file.seekg(0, std::ios::beg);
    file.read((char*)&buffer[0], size);
    file.close();
    return buffer;
}

std::string Cartridge::getTitle(const std::vector<unsigned char>& buffer)
{
	auto titleStart = (char*)&buffer[0x134];
	auto len = strnlen(titleStart, 16);
	std::string title(titleStart, len);
	return title;
}
