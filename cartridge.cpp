#include "cartridge.h"
#include <fstream>

#include <iostream>

Cartridge::Cartridge(const char *filename)
    : _filename(filename)
{}


std::unique_ptr<IMBC> Cartridge::getMBC() {
    std::cout << "getMBC" << std::endl;
    std::vector<unsigned char> buffer = readFile(_filename);
    unsigned char mbcType = getCartridgeType(buffer);
    std::cout << "cartridge type:" << mbcType << std::endl;
    size_t ramSize = getRamSize(buffer);
    std::cout << "Ram size:" << ramSize << std::endl;
    std::vector<unsigned char> ram(ramSize, 0);

    switch (mbcType)
    {
        case 0x01:
        case 0x02:
        case 0x03:
            return std::unique_ptr<IMBC>(new MBC1(buffer, ram));
        default:
            return std::unique_ptr<IMBC>();

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
    std::streampos size = file.tellg();
    std::vector<unsigned char> buffer(size);
    file.seekg(0, std::ios::beg);
    file.read((char*)&buffer[0], size);
    file.close();
    return buffer;
}
