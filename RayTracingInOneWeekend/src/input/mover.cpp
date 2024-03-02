#include "mover.h"

#include "SDL_mouse.h"

bool Mover::UpdateTranform(point3& position, vec3& rotation, const Input& input, double deltaTime)
{
	bool positionHasChanged = UpdatePosition(position, rotation, input, deltaTime);
	bool rotationHasChanged = UpdateRotation(position, rotation, input, deltaTime);

	return positionHasChanged || rotationHasChanged;
}

bool Mover::UpdatePosition(point3& position, vec3& rotation, const Input& input, double deltaTime)
{
	bool hasChanged = false;
	vec3 change = { 0,0,0 };

	if (input.Forward())
	{
		change += vec3(0, 0, -m_movementSpeed) * deltaTime;
		hasChanged = true;
	}
	if (input.Backward())
	{
		change += vec3(0, 0, m_movementSpeed) * deltaTime;
		hasChanged = true;
	}
	if (input.Left())
	{
		change += vec3(-m_movementSpeed, 0, 0) * deltaTime;
		hasChanged = true;
	}
	if (input.Right())
	{
		change += vec3(m_movementSpeed, 0, 0) * deltaTime;
		hasChanged = true;
	}
	if (input.Up())
	{
		change += vec3(0, m_movementSpeed, 0) * deltaTime;
		hasChanged = true;
	}
	if (input.Down())
	{
		change += vec3(0, -m_movementSpeed, 0) * deltaTime;
		hasChanged = true;
	}

	if (input.LeftShift())
	{
		change *= m_speedMultiplier;
	}

	change.Rotate(rotation);
	position += change;
	return hasChanged;
}

bool Mover::UpdateRotation(point3& position, vec3& rotation, const Input& input, double deltaTime)
{
	bool hasChanged = false;	

	if (input.RightClick())
	{
		SDL_ShowCursor(false);
		int prevMouseX = mouseX;
		int prevMouseY = mouseY;

		SDL_GetMouseState(&mouseX, &mouseY);
		int deltaMouseX = mouseX - prevMouseX;
		int deltaMouseY = mouseY - prevMouseY;

		//std::cout << deltaMouseX << " " << deltaMouseY << std::endl;

		double yAngle = -(double)deltaMouseX * deltaTime * 0.5;
		double xAngle = -(double)deltaMouseY * deltaTime * 0.5;
		vec3 eulerAngles = { xAngle, yAngle, 0 };

		rotation += vec3(xAngle, yAngle, 0);

		hasChanged = true;
		//std::cout << yAngle << std::endl;
	}
	else 
	{
		SDL_ShowCursor(true);
		SDL_GetMouseState(&mouseX, &mouseY);
	}

	return hasChanged;
}
