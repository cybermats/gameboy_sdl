#include "gbu_sdl_wrapper.h"
#include "gbu.h"
#include "joypad.h"
#include <cassert>

GbuSdlWrapper::GbuSdlWrapper(Gbu* gbu)
	: SDL_Wrapper(160 * 4, 144 * 4, 160, 144, 15)
	, _gbu(gbu)
{
}

bool GbuSdlWrapper::Display(std::vector<uint32_t> &buffer) {
	_gbu->runFrame();
	const auto& frame = _gbu->getBuffer();
	assert(frame.size() == buffer.size());
	for (size_t i = 0; i < frame.size(); ++i)
	{
		auto c = frame[i];
		unsigned char r = c;
		unsigned char g = (unsigned char)(c*(240.0 / 255));
		unsigned char b = (unsigned char)(c*(180.0 / 255));
		buffer[i] = b | g << 8 | r << 16 | 0xff << 24;
	}
	return true;
}

void GbuSdlWrapper::KeyDown(SDL_Keysym keysym) {
	switch (keysym.sym)
	{
	case SDLK_UP:
		std::cout << "KeyDown Up" << std::endl;
		_gbu->joypad()->KeyDown(Joypad::Up);
		break;
	case SDLK_DOWN:
		std::cout << "KeyDown Down" << std::endl;
		_gbu->joypad()->KeyDown(Joypad::Down);
		break;
	case SDLK_LEFT:
		std::cout << "KeyDown Left" << std::endl;
		_gbu->joypad()->KeyDown(Joypad::Left);
		break;
	case SDLK_RIGHT:
		std::cout << "KeyDown Right" << std::endl;
		_gbu->joypad()->KeyDown(Joypad::Right);
		break;
	case SDLK_o:
		std::cout << "KeyDown A" << std::endl;
		_gbu->joypad()->KeyDown(Joypad::A);
		break;
	case SDLK_p:
		std::cout << "KeyDown B" << std::endl;
		_gbu->joypad()->KeyDown(Joypad::B);
		break;
	case SDLK_RETURN:
		std::cout << "KeyDown Start" << std::endl;
		_gbu->joypad()->KeyDown(Joypad::Start);
		break;
	case SDLK_SPACE:
		std::cout << "KeyDown Select" << std::endl;
		_gbu->joypad()->KeyDown(Joypad::Select);
		break;
	default:
		break;
	}
}

void GbuSdlWrapper::KeyUp(SDL_Keysym keysym) {
	switch (keysym.sym)
	{
	case SDLK_UP:
		std::cout << "KeyUp Up" << std::endl;
		_gbu->joypad()->KeyUp(Joypad::Up);
		break;
	case SDLK_DOWN:
		std::cout << "KeyUp Down" << std::endl;
		_gbu->joypad()->KeyUp(Joypad::Down);
		break;
	case SDLK_LEFT:
		std::cout << "KeyUp Left" << std::endl;
		_gbu->joypad()->KeyUp(Joypad::Left);
		break;
	case SDLK_RIGHT:
		std::cout << "KeyUp Right" << std::endl;
		_gbu->joypad()->KeyUp(Joypad::Right);
		break;
	case SDLK_o:
		std::cout << "KeyUp A" << std::endl;
		_gbu->joypad()->KeyUp(Joypad::A);
		break;
	case SDLK_p:
		std::cout << "KeyUp B" << std::endl;
		_gbu->joypad()->KeyUp(Joypad::B);
		break;
	case SDLK_RETURN:
		std::cout << "KeyUp Start" << std::endl;
		_gbu->joypad()->KeyUp(Joypad::Start);
		break;
	case SDLK_SPACE:
		std::cout << "KeyUp Select" << std::endl;
		_gbu->joypad()->KeyUp(Joypad::Select);
		break;
	default:
		break;
	}

}
