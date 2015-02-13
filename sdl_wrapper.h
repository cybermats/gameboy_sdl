#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <SDL2/SDL_keyboard.h>

class SDL_Wrapper
{
public:
    SDL_Wrapper(int width, int height, int textureWidth, int textureHeight, int interval);
     ~SDL_Wrapper();

    void mainloop();


private:
    bool OnDisplay(std::vector<uint32_t>& buffer) {
        return Display(buffer);
    }

    virtual bool Display(std::vector<uint32_t>& buffer) = 0;
    virtual void KeyDown(SDL_Keysym keysym) {}
    virtual void KeyUp(SDL_Keysym keysym) {}

    friend class SDL_Wrapper_Impl;
    class SDL_Wrapper_Impl;

    std::unique_ptr<SDL_Wrapper_Impl> _pimpl;

};

