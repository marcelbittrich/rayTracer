#pragma once

#include "uidata.h"
#include "imgui.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_impl_sdl2.h"
#include <stdio.h>

#include "../tools/vec3.h"
#include "../objects/hittablelist.h"

class UI
{
public:
	UI(SDL_Window* window, SDL_Renderer* renderer);
	~UI();
	void Update(UIData& data, HittableList& world);
	void Render();

private:
	struct SDL_Window* m_window;
	struct SDL_Renderer* m_renderer;
};

