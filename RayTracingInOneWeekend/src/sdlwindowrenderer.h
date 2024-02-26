#pragma once

#include "SDL_render.h"

#include "tools/rtweekend.h"
#include "tools/windowinfo.h"
#include "tools/color.h"

class SDLWindowRenderer
{
public:
	SDLWindowRenderer(SDL_Renderer* renderer) : m_renderer(renderer) {};
	void Render(color imageBuffer[], const WindowInfo& windowInfo);

private:
	SDL_Renderer* m_renderer;
	void SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
};

