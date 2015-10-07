#pragma once

#include <stdint.h>


class Audio
{
public:
	unsigned char read(unsigned short addr)
	{
		uint8_t result = 0;
		switch (addr)
		{
		case 0xff10:
			result |= (c1_sweep_time & 0x07) << 4;
			result |= c1_sweep_decrease ? (1 << 3) : 0;
			result |= c1_num_sweep_shift & 0x07;
			break;
		case 0xff11:
			result |= (c1_wave_pattern_duty & 0x3) << 6;
			result |= (c1_sound_length_data & 0x1f);
			break;
		case 0xff12:
			result |= (c1_init_envelope_volume & 0x0f) << 4;
			result |= c1_envelope_increase ? (1 << 3) : 0;
			result |= c1_num_envelope_sweep & 0x07;
			break;
		case 0xff13:
			result |= c1_frequency & 0xff;
			break;
		case 0xff14:
			result |= c1_restart_sound ? (1 << 7) : 0;
			result |= c1_counter_selection ? (1 << 6) : 0;
			result |= (c1_frequency >> 8) & 0x07;
			break;

		case 0xff24:
			result |= (s02_output_level & 0x07) << 4;
			result |= (s01_output_level & 0x07);
			break;

		case 0xff25:
			result = (c4_to_s02 ? (1 << 7) : 0) |
				(c3_to_s02 ? (1 << 6) : 0) |
				(c2_to_s02 ? (1 << 5) : 0) |
				(c1_to_s02 ? (1 << 4) : 0) |
				(c4_to_s01 ? (1 << 3) : 0) |
				(c3_to_s01 ? (1 << 2) : 0) |
				(c2_to_s01 ? (1 << 1) : 0) |
				(c1_to_s01 ? (1 << 0) : 0);
			break;

		case 0xff26:
			result =
				(sound_on ? (1 << 7) : 0) |
				(sound4_on ? (1 << 3) : 0) |
				(sound3_on ? (1 << 2) : 0) |
				(sound2_on ? (1 << 1) : 0) |
				(sound1_on ? (1 << 0) : 0);
			break;
		default:
			break;
		}
		return result;
	}

	void write(unsigned short addr, unsigned char value)
	{
		switch (addr)
		{
		case 0xff10:
			c1_sweep_time = (value >> 4) & 0x07;
			c1_sweep_decrease = (value & (1 << 3)) != 0;
			c1_num_sweep_shift = value & 0x07;
			break;
		case 0xff11:
			c1_wave_pattern_duty = (value >> 6) & 0x3;
			c1_sound_length_data = value & 0x1f;
			break;
		case 0xff12:
			c1_init_envelope_volume = (value >> 4) & 0x0f;
			c1_envelope_increase = (value & (1 << 3)) != 0;
			c1_num_envelope_sweep = value & 0x07;
			break;
		case 0xff13:
			c1_frequency = (c1_frequency & 0x0700) | value;
			break;
		case 0xff14:
			c1_restart_sound = (value & (1 << 7)) != 0;
			c1_counter_selection = (value & (1 << 6)) != 0;
			c1_frequency = (c1_frequency & 0x00ff) | ((value & 0x07) << 8);
			break;


		case 0xff24:
			s02_output_level = (value >> 4) & 0x07;
			s01_output_level = (value & 0x07);
			break;

		case 0xff25:
			c4_to_s02 = (value & (1 << 7)) != 0;
			c3_to_s02 = (value & (1 << 6)) != 0;
			c2_to_s02 = (value & (1 << 5)) != 0;
			c1_to_s02 = (value & (1 << 4)) != 0;
			c4_to_s01 = (value & (1 << 3)) != 0;
			c3_to_s01 = (value & (1 << 2)) != 0;
			c2_to_s01 = (value & (1 << 1)) != 0;
			c1_to_s01 = (value & (1 << 0)) != 0;
			break;

		case 0xff26:
			sound_on = (value & (1 << 7)) != 0;
			break;
		default:
			break;
		}
	}

	void tick(uint16_t cycles)
	{

	}

	// FF10
	uint8_t c1_sweep_time;
	bool c1_sweep_decrease;
	uint8_t c1_num_sweep_shift;
	// FF11
	uint8_t c1_wave_pattern_duty;
	uint8_t c1_sound_length_data;
	// FF12
	uint8_t c1_init_envelope_volume;
	bool c1_envelope_increase;
	uint8_t c1_num_envelope_sweep;
	// FF13
	uint16_t c1_frequency;
	// FF14 (and FF13)
	bool c1_restart_sound;
	bool c1_counter_selection;

	// FF16
	uint8_t c2_wave_pattern_duty;
	uint8_t c2_sound_length_data;
	// FF17
	uint8_t c2_init_envelope_volume;
	bool c2_envelope_increase;
	uint8_t c2_num_envelope_sweep;
	// FF18
	uint16_t c2_frequency;
	// FF19 (and FF18)
	bool c2_restart_sound;
	bool c2_counter_selection;

	// FF1A
	bool c3_playback;
	// FF1B
	uint8_t c3_sound_length;
	// FF1C
	uint8_t c3_output_level;
	// FF1D
	uint16_t c3_frequency;
	// FF1E
	bool c3_restart_sound;
	bool c3_counter_selection;

	// FF20
	uint8_t c4_sound_length_data;
	// FF21
	uint8_t c4_init_envelope_volume;
	bool c4_envelope_increase;
	uint8_t c4_num_envelope_sweep;
	// FF22
	uint8_t c4_shift_clock_freq;
	bool c4_counter;
	uint8_t c4_dividing_ratio;
	// FF23
	bool c4_restart_sound;
	bool c4_counter_selection;

	// FF24
	uint8_t s02_output_level;
	uint8_t s01_output_level;

	// FF25
	bool c4_to_s02;
	bool c3_to_s02;
	bool c2_to_s02;
	bool c1_to_s02;
	bool c4_to_s01;
	bool c3_to_s01;
	bool c2_to_s01;
	bool c1_to_s01;

	// FF26
	bool sound_on;
	bool sound4_on;
	bool sound3_on;
	bool sound2_on;
	bool sound1_on;

	// FF30-FF3F
	uint8_t wave_pattern[0x0f];


};