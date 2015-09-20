#pragma once

#include "gbu.h"
#include "joypad.h"
#include "sdl_wrapper.h"
#include <SDL_keyboard.h>
#include <cassert>


class GbuSdlWrapper : public SDL_Wrapper {


public:
    GbuSdlWrapper(Gbu* gbu)
            : SDL_Wrapper(640, 480, 160, 144, 10)
    , _gbu(gbu)
    {
    }

private:
    virtual bool Display(std::vector<uint32_t> &buffer) override {
        _gbu->runFrame();
        const auto& frame = _gbu->getBuffer();
        assert(frame.size() == buffer.size());
        for(size_t i = 0; i < frame.size(); ++i)
        {
            auto c = frame[i];
			unsigned char r = c;
			unsigned char g = c*(240.0/255);
			unsigned char b = c*(180.0/255);
            buffer[i] = b | g << 8 | r << 16 | 0xff << 24;
        }
        return true;
    }

    virtual void KeyDown(SDL_Keysym keysym) override {
		switch (keysym.sym)
		{
		case SDLK_w:
			_gbu->joypad()->KeyDown(Joypad::Up);
			break;
		case SDLK_s:
			_gbu->joypad()->KeyDown(Joypad::Down);
			break;
		case SDLK_a:
			_gbu->joypad()->KeyDown(Joypad::Left);
			break;
		case SDLK_d:
			_gbu->joypad()->KeyDown(Joypad::Right);
			break;
		case SDLK_o:
			_gbu->joypad()->KeyDown(Joypad::A);
			break;
		case SDLK_p:
			_gbu->joypad()->KeyDown(Joypad::B);
			break;
		case SDLK_RETURN:
			_gbu->joypad()->KeyDown(Joypad::Start);
			break;
		case SDLK_SPACE:
			_gbu->joypad()->KeyDown(Joypad::Select);
			break;
		default:
			break;
		}
    }

    virtual void KeyUp(SDL_Keysym keysym) override {
		switch (keysym.sym)
		{
		case SDLK_w:
			_gbu->joypad()->KeyUp(Joypad::Up);
			break;
		case SDLK_s:
			_gbu->joypad()->KeyUp(Joypad::Down);
			break;
		case SDLK_a:
			_gbu->joypad()->KeyUp(Joypad::Left);
			break;
		case SDLK_d:
			_gbu->joypad()->KeyUp(Joypad::Right);
			break;
		case SDLK_o:
			_gbu->joypad()->KeyUp(Joypad::A);
			break;
		case SDLK_p:
			_gbu->joypad()->KeyUp(Joypad::B);
			break;
		case SDLK_RETURN:
			_gbu->joypad()->KeyUp(Joypad::Start);
			break;
		case SDLK_SPACE:
			_gbu->joypad()->KeyUp(Joypad::Select);
			break;
		default:
			break;
		}

    }

    Gbu* _gbu;
};

