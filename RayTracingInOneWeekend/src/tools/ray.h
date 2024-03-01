#pragma once

#include "vec3.h"

class Ray
{
public:
	Ray() {}

	Ray(const point3& origin, const vec3& direction)
		: m_origin(origin), m_direction(direction) {}

	point3 origin() const { return m_origin; }
	vec3 direction() const { return m_direction; }

	point3 at(double t) const {
		return m_origin + m_direction * t; 
	}

private:
	point3 m_origin;
	vec3   m_direction;
};