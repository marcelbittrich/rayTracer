#include "input.h"

#include "SDL.h"
#include "imgui_impl_sdl2.h"

void Input::HandleInput(bool& isRunning)
{
	type.windowResize = false;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				type.windowResize = true;
			}
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_UP:
			case SDLK_w:
				type.forward = true;
				break;
			case SDLK_DOWN:
			case SDLK_s:
				type.backward = true;
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				type.right = true;
				break;
			case SDLK_LEFT:
			case SDLK_a:
				type.left = true;
				break;
			case SDLK_q:
				type.up = true;
				break;
			case SDLK_e:
				type.down = true;
				break;
			case SDLK_LSHIFT:
				type.leftShift = true;
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_UP:
			case SDLK_w:
				type.forward = false;
				break;
			case SDLK_DOWN:
			case SDLK_s:
				type.backward = false;
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				type.right = false;
				break;
			case SDLK_LEFT:
			case SDLK_a:
				type.left = false;
				break;
			case SDLK_q:
				type.up = false;
				break;
			case SDLK_e:
				type.down = false;
				break;
			case SDLK_LSHIFT:
				type.leftShift = false;
				break;
			default:
				break;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				type.LeftClick = true;
				break;
			case SDL_BUTTON_RIGHT:
				type.RightClick = true;
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				type.LeftClick = false;
				break;
			case SDL_BUTTON_RIGHT:
				type.RightClick = false;
				break;
			default:
				break;
			}
			break;

		//case SDL_CONTROLLERBUTTONDOWN:
		//	switch (event.cbutton.button)
		//	{
		//	case SDL_CONTROLLER_BUTTON_B:
		//		break;
		//	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		//		break;
		//	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		//		break;
		//	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		//		break;
		//	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		//		break;
		//	default:
		//		break;
		//	}
		//	break;
		//case SDL_CONTROLLERBUTTONUP:
		//	switch (event.cbutton.button)
		//	{
		//	case SDL_CONTROLLER_BUTTON_B:
		//		break;
		//	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		//		break;
		//	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		//		break;
		//	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		//		break;
		//	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		//		break;
		//	default:
		//		break;
		//	}
		//	break;

		default:
			break;
		}
	}
}
