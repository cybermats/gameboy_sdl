#pragma once

#include "sdl_wrapper.h"
#include "Sound_Queue.h"
#include <SDL_keyboard.h>

class Gbu;

class GbuSdlWrapper : public SDL_Wrapper {
public:
	GbuSdlWrapper(Gbu* gbu);

private:
	virtual bool Display(std::vector<uint32_t> &buffer) override;
	virtual void KeyDown(SDL_Keysym keysym) override;
	virtual void KeyUp(SDL_Keysym keysym) override;

    Gbu* _gbu;
	Sound_Queue _sound;
	std::vector<short> _sound_buffer;
};

