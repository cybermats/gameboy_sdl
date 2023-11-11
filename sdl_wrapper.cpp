#include "sdl_wrapper.h"

#include <SDL.h>
#include <iostream>
#include <vector>


class SDL_Wrapper::SDL_Wrapper_Impl
{
public:
    SDL_Wrapper_Impl()
        : _width(0)
        , _height(0)
          , _textureWidth(0)
          , _textureHeight(0)
        , _interval(10)
        , _window(nullptr)
        , _renderer(nullptr)
        , _texture(nullptr)
        , _initialized(false)
        , _owner(nullptr)
    {
    }

    ~SDL_Wrapper_Impl()
    {
        if(_texture)
            SDL_DestroyTexture(_texture);
        if(_renderer)
            SDL_DestroyRenderer(_renderer);
        if(_window)
            SDL_DestroyWindow(_window);
        SDL_Quit();
    }

    bool init(
            int width,
            int height,
            int textureWidth,
            int textureHeight,
            int interval,
            SDL_Wrapper* owner
    )
    {
        _width = width;
        _height = height;
        _textureWidth = textureWidth;
        _textureHeight = textureHeight;
        _buffer.resize(_textureWidth * _textureHeight);
        _owner = owner;

        if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return false;
        }
		atexit(SDL_Quit);

        if(SDL_CreateWindowAndRenderer(
                _width,
                _height,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC,
                &_window,
                &_renderer))
        {
            std::cout << "SDL_CreateWindowAndRenderer Error: " << SDL_GetError() << std::endl;
            return false;
        }

        _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, _textureWidth, _textureHeight);
        if(!_texture)
        {
            std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
            return false;
        }

        _interval = interval;
        _initialized = true;
        return true;
    }

    void mainloop()
    {
        if(!_initialized)
        {
            std::cout << "Not successfully initialized." << std::endl;
            return;
        }

        auto timerId = SDL_AddTimer(_interval, TimerCallback, nullptr);

        SDL_Event e;
        while(SDL_WaitEvent(&e)) {
            if(e.type == SDL_QUIT)
                return;
            if(e.type == SDL_KEYDOWN)
            {
                if(e.key.repeat)
                    continue;

                switch (e.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        SDL_Quit();
                        return;
                    default:
                        _owner->KeyDown(e.key.keysym);
                        break;
                }
            }
            if(e.type == SDL_KEYUP)
            {
                _owner->KeyUp(e.key.keysym);
            }
            if(e.type == SDL_USEREVENT)
            {
                OnDisplay();
            }
        }

    }

private:

    void OnDisplay()
    {
        // Call the callback
        if(!_owner->OnDisplay(_buffer))
            return;

        if(SDL_UpdateTexture(_texture, nullptr, &_buffer[0], _textureWidth*(sizeof(Uint32))))
        {
            std::cout << "SDL_UpdateTexture Error: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_RenderClear(_renderer);
        SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
        SDL_RenderPresent(_renderer);
    }

    static Uint32 TimerCallback(Uint32 interval, void *param)
    {
        SDL_Event event;
        SDL_zero(event);
        event.type = SDL_USEREVENT;
        SDL_PushEvent(&event);
        return interval;
    }

    int _width;
    int _height;
    int _textureWidth;
    int _textureHeight;
    int _interval;

    SDL_Window* _window;
    SDL_Renderer* _renderer;
    SDL_Texture* _texture;

    std::vector<uint32_t> _buffer;

    bool _initialized;

    SDL_Wrapper* _owner;
};


SDL_Wrapper::SDL_Wrapper(int width, int height, int textureWidth, int textureHeight, int interval)
    : _pimpl(new SDL_Wrapper_Impl)
{
    _pimpl->init(width, height, textureWidth, textureHeight, interval, this);
}

SDL_Wrapper::~SDL_Wrapper() = default;

void SDL_Wrapper::mainloop() {
    _pimpl->mainloop();
}

