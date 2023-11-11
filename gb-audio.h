#pragma once

#include <stdint.h>
#include "gb_apu\Gb_Apu.h"
#include "gb_apu\Multi_Buffer.h"

class Audio
{
public:
	Audio()
	{
		time = 0;
		apu.treble_eq(-20.0);
		buf.bass_freq(461);
		apu.output(buf.center(), buf.left(), buf.right());
		buf.clock_rate(4194304);
		buf.set_sample_rate(44100);
	}

	unsigned char read(unsigned short addr)
	{
		return (unsigned char)apu.read_register(time, addr);
	}

	void write(unsigned short addr, unsigned char value)
	{
		apu.write_register(time, addr, value);
	}

	void tick(uint16_t cycles)
	{
		time += (cycles * 4);
	}

	void end_frame()
	{
		time = 0;
		bool stereo = apu.end_frame(frame_length);
		buf.end_frame(frame_length, stereo);
	}

	long samples_available() const
	{
		return buf.samples_avail();
	}

	long read_samples(blip_sample_t* out, long count)
	{
		return buf.read_samples(out, count);
	}

	Gb_Apu apu;
	Stereo_Buffer buf;
	blip_time_t time;

private:
	gb_time_t const frame_length = 70224;
};