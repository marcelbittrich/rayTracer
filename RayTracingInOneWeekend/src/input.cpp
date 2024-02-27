#include "input.h"

#include "SDL.h"

void Input::HandleInput(bool& isRunning)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_UP:
			case SDLK_w:
				type.up = true;
				break;
			case SDLK_DOWN:
			case SDLK_s:
				type.down = true;
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				type.right = true;
				break;
			case SDLK_LEFT:
			case SDLK_a:
				type.left = true;
				break;
			case SDLK_LSHIFT:
				type.leftShift = true;
				break;
			//case SDLK_SPACE:
			//	m_controlBools.isShooting = true;
			//	break;
			//case SDLK_LCTRL:
			//	m_controlBools.isUsingBomb = true;
			//	break;
			//case SDLK_p:
			//case SDLK_ESCAPE:
			//	m_controlBools.pausePressed = true;
			//	break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_UP:
			case SDLK_w:
				type.up = false;
				break;
			case SDLK_DOWN:
			case SDLK_s:
				type.down = false;
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				type.right = false;
				break;
			case SDLK_LEFT:
			case SDLK_a:
				type.left = false;
				break;
			case SDLK_LSHIFT:
				type.leftShift = false;
				break;
			//case SDLK_SPACE:
			//	m_controlBools.isShooting = false;
			//	break;
			//case SDLK_LCTRL:
			//	m_controlBools.isUsingBomb = false;
			//	break;
			//case SDLK_p:
			//case SDLK_ESCAPE:
			//	m_controlBools.pausePressed = false;
			//	break;
			default:
				break;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				type.leftClick = true;
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				type.leftClick = false;
				break;
			default:
				break;
			}
			break;

		//case SDL_CONTROLLERBUTTONDOWN:
		//	switch (event.cbutton.button)
		//	{
		//	case SDL_CONTROLLER_BUTTON_B:
		//		m_controlBools.giveThrust = true;
		//		break;
		//	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		//		m_controlBools.isTurningRight = true;
		//		break;
		//	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		//		m_controlBools.isTurningLeft = true;
		//		break;
		//	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		//		m_controlBools.isShooting = true;
		//		break;
		//	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		//		m_controlBools.isUsingBomb = true;
		//		break;
		//	default:
		//		break;
		//	}
		//	break;
		//case SDL_CONTROLLERBUTTONUP:
		//	switch (event.cbutton.button)
		//	{
		//	case SDL_CONTROLLER_BUTTON_B:
		//		m_controlBools.giveThrust = false;
		//		break;
		//	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		//		m_controlBools.isTurningRight = false;
		//		break;
		//	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		//		m_controlBools.isTurningLeft = false;
		//		break;
		//	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		//		m_controlBools.isShooting = false;
		//		break;
		//	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		//		m_controlBools.isUsingBomb = false;
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
