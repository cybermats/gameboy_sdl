#pragma once

#include "mbc.h"

#include <vector>
#include <memory>


class Cartridge
{
public:
	Cartridge(const char* filename);
	std::unique_ptr<IMBC> getMBC();

private:
	const char* _filename;
	std::string _title;

    std::vector<unsigned char> readFile(const char* filename);
    static unsigned char getCartridgeType(const std::vector<unsigned char>& buffer);
    static size_t getRamSize(const std::vector<unsigned char>& buffer);
	static std::string getTitle(const std::vector<unsigned char>& buffer);


};