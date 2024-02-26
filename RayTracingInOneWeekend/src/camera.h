#pragma once

#include "memory"

#include "tools/rtweekend.h"
#include "objects/hittablelist.h"
#include "tools/windowInfo.h"
#include "tools/color.h"


class Camera
{
public:
	Camera(const WindowInfo& windowInfo);

	void HandleInput() {};

	void Update(const HittableList& world, color imageBuffer[], const WindowInfo& windowInfo);

	point3 GetPosition() const { return m_position; }
	vec3 GetViewDirection() const{ return m_viewDirection; }


private:
	double m_focalLength;
	vec3 m_pixelDeltaU;
	vec3 m_pixelDeltaV;
	point3 m_firstPixelLocation;
	double m_viewportHeight;
	double m_viewportWidth;
	point3 m_position;
	vec3 m_viewDirection;

	int m_sampleCount = 0;
	
	Ray GetRay(int i, int j);
	vec3 PixelSampleSquare();
	color RayColor(const Ray& ray, const Hittable& world);
};

