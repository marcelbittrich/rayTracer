#pragma once

#include "memory"

#include "input/input.h"
#include "input/mover.h"
#include "tools/rtweekend.h"
#include "objects/hittablelist.h"
#include "tools/windowInfo.h"
#include "tools/color.h"

class Camera
{
public:
	Camera(const WindowInfo& windowInfo);

	void HandleInput(const Input& input, double deltaTime);
	void Update(const HittableList& world, color imageBuffer[], const WindowInfo& windowInfo);

	point3 GetPosition() const { return m_position; }
	vec3 GetRotation() const{ return m_rotation; }


private:
	int    m_maxBounce = 10;
	double hfov        = 60;
	double m_focalLength;
	vec3 m_pixelDeltaU;
	vec3 m_pixelDeltaV;
	point3 m_firstPixelLocation;
	double m_viewportHeight;
	double m_viewportWidth;
	point3 m_position;
	vec3 m_rotation = { 0,0,0 };
	vec3 m_viewDirection = {0,0,-1};

	int m_sampleCount = 0;
	uint32_t m_seed = 0;
	bool m_HasChanged = false;

	double m_movementSpeed = 1.0;
	double m_rotatingSpeed = 1.0;
	double m_speedMultiplier = 2.0;
	Mover m_mover = {
		m_movementSpeed,
		m_rotatingSpeed,
		m_speedMultiplier
	};
	
	void RecalculateViewport(const WindowInfo& windowInfo);
	Ray GetRay(int i, int j);
	vec3 PixelSampleSquare();
	color RayColor(const Ray& ray, int maxBounce, const Hittable& world, uint32_t seed);
};