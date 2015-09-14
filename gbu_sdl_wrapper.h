#pragma once

#include "gbu.h"
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
        for(auto i = 0; i < frame.size(); ++i)
        {
            auto c = frame[i];
//            buffer[i] = c | c << 8 | c << 16 | c << 24;
            if(c == 255)
                buffer[i] = 0xFFFFFFFF;
            else
                buffer[i] = 0;
//            buffer[i] = c;
        }
        return true;
    }

    virtual void KeyDown(SDL_Keysym keysym) override {

    }

    virtual void KeyUp(SDL_Keysym keysym) override {

    }

    Gbu* _gbu;
};

