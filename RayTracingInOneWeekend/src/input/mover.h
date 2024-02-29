#pragma once

#include "input.h"
#include "../tools/vec3.h"

class Mover
{
public:
	Mover(double translationSpeed, double rotationSpeed, double speedMultiplier)
		: m_movementSpeed(translationSpeed), m_rotationSpeed{ rotationSpeed }, m_speedMultiplier(speedMultiplier) {};

	bool UpdateTranform(point3& position, vec3& rotation, const Input& input, double deltaTime);

private:
	double m_movementSpeed;
	double m_rotationSpeed;
	double m_speedMultiplier;
	int mouseX = 0;
	int mouseY = 0;
	vec3 m_eulerAngles = { 0,0,0, };

	bool UpdatePosition(point3& position, vec3& rotation, const Input& input, double deltaTime);
	bool UpdateRotation(point3& position, vec3& rotation, const Input& input, double deltaTime);
};