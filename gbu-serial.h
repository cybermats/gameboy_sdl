#pragma once

#include <stdint.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>

class Serial
{
public:
	Serial()
	{
		_serial_stream.open("serialstream.txt");
	}

	~Serial()
	{
		_serial_stream.close();
	}


	unsigned char read(unsigned short addr)
	{
		switch (addr)
		{
		case 0xff01:
			return sb;
		case 0xff02:
			return sc;
		}
		assert(false);
		return 0;
	}
	void write(unsigned short addr, unsigned char value)
	{
		switch (addr)
		{
		case 0xff01:
			sb = value;
			_serial_stream << sb;
			_serial_stream.flush();
			return;
		case 0xff02:
			sc = value;
			return;
		}
		assert(false);
	}
	uint8_t sb;
	uint8_t sc;
	std::ofstream _serial_stream;


};