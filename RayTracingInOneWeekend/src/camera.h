#pragma once

#include "tools/vec3.h"

class Camera
{
public:
	Camera() : m_position(vec3(0.0, 0.0, 4.0)) {}

	point3 GetPosition() const { return m_position; }
	vec3 GetViewDirection() const{ return m_viewDirection; }

private:
	point3 m_position;
	vec3 m_viewDirection;
};

