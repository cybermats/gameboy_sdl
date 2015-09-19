#pragma once

#include "interrupts.h"

class Joypad
{
public:
	enum Key {
		None = 0,
		Right = (1 << 0),
		Left = (1 << 1),
		Up = (1 << 2),
		Down = (1 << 3),
		A = (1 << 4),
		B = (1 << 5),
		Select = (1 << 6),
		Start = (1 << 7),
	};

	Joypad(Interrupts* interrupts)
		: _interrupts(interrupts)
		, _keyState(None)
		, _p14State(false)
		, _p15State(false)
	{}

	void KeyDown(Key key)
	{
		_keyState = (Key)(_keyState | key);
		if (_p14State || _p15State)
			_interrupts->flagInterrupt(Interrupts::Joypad);
	}

	void KeyUp(Key key)
	{
		_keyState = (Key)(_keyState & ~key);

	}

	void write(uint8_t value)
	{
		_p14State = (value & (1 << 4)) == 0;
		_p15State = (value & (1 << 5)) == 0;
	}

	uint8_t read()
	{
		// Set the in-lines (Button/Direction bits) to 0 if button is pressed.

		uint8_t result = 0xFF;
		result &= _p15State ? 0xff : ~(1 << 5);
		result &= _p14State ? 0xff : ~(1 << 4);

		if (_p14State)
		{
			if (_keyState & Right)
				result &= ~(1 << 0);
			if (_keyState & Left)
				result &= ~(1 << 1);
			if (_keyState & Up)
				result &= ~(1 << 2);
			if (_keyState & Down)
				result &= ~(1 << 3);
		}
		if (_p15State)
		{
			if (_keyState & A)
				result &= ~(1 << 0);
			if (_keyState & B)
				result &= ~(1 << 1);
			if (_keyState & Select)
				result &= ~(1 << 2);
			if (_keyState & Start)
				result &= ~(1 << 3);
		}

		return result;
	}
	

private:
	Interrupts* _interrupts;
	Key _keyState;
	bool _p14State; // If p14 is selected, i.e. == 0
	bool _p15State; // If p15 is selected, i.e. == 0
};