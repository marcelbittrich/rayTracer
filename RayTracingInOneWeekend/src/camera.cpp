#include "camera.h"

#include <algorithm>
#include <execution>

#include "SDL_mouse.h"

#include "objects/material.h"

#define Multithreading 1

Camera::Camera(const WindowInfo& windowInfo)
	: m_position(vec3(0.0, 0.0, 4.0))
{
	RecalculateViewport(windowInfo);
}

void Camera::RecalculateViewport(const WindowInfo& windowInfo)
{
#if Multithreading
	// Set itterator for multithreading
	horizontalIter.resize(windowInfo.width);
	verticalIter.resize(windowInfo.height);
	for (uint32_t i = 0; i < (uint32_t)windowInfo.width; i++)
		horizontalIter[i] = i;
	for (uint32_t i = 0; i < (uint32_t)windowInfo.height; i++)
		verticalIter[i] = i;
#endif

	// Calculation of the vieport local to camera
	double theta = degreesToRadians(hfov);
	double h = tan(theta / 2);

	m_viewportWidth = 2 * h * m_focusDistance;
	m_viewportHeight = m_viewportWidth * ((double)(windowInfo.height) / windowInfo.width);

	vec3 viewportU = { m_viewportWidth, 0, 0 };
	vec3 viewportV = { 0, -m_viewportHeight, 0 };

	m_pixelDeltaU = viewportU / windowInfo.width;
	m_pixelDeltaV = viewportV / windowInfo.height;

	vec3 startViewDirection = { 0, 0, -1 };
	point3 focusPoint = startViewDirection * m_focusDistance;

	// Calculate location of upper left pixel 
	point3 viewportUpperLeft = focusPoint - viewportU / 2 - viewportV / 2;
	m_firstPixelLocation = viewportUpperLeft + m_pixelDeltaU / 2 + m_pixelDeltaV / 2;

	// Translate and Rotate into world space
	vec3 m_localFirstPixelLocation = m_firstPixelLocation;
	m_localFirstPixelLocation.Rotate(m_rotation);

	m_firstPixelLocation = m_position + m_localFirstPixelLocation;
	m_pixelDeltaU = m_pixelDeltaU.Rotate(m_rotation);
	m_pixelDeltaV = m_pixelDeltaV.Rotate(m_rotation);

	// Calculate camera defocus disk basis vectors;
	double defocus_raidus = m_focusDistance * tan(degreesToRadians(m_defocusAngle / 2));
	m_defocusDistanceU = unitVector(m_pixelDeltaU) * defocus_raidus;
	m_defocusDistanceV = unitVector(m_pixelDeltaV) * defocus_raidus;
}

void Camera::HandleInput(const Input& input, double deltaTime)
{
	if (input.LeftClick())
	{
		m_setFocusToMouse = true;
	}

	m_hasChanged = m_mover.UpdateTranform(m_position, m_rotation, input, deltaTime);
}

void Camera::Update(const HittableList& world, color* imageBuffer, const WindowInfo& windowInfo)
{
	m_currentWorld = &world;
	m_currentWindowInfo = &windowInfo;
	m_ImageBuffer = imageBuffer;

	if (m_hasFocusBlur && m_setFocusToMouse)
	{
		double prevFocusDistance = m_focusDistance;
		m_focusDistance = GetFocusDistanceOnClick(world);
		std::cout << m_focusDistance << std::endl;
		m_hasChanged = prevFocusDistance != m_focusDistance;
		m_setFocusToMouse = false;
	}

	if (m_hasChanged)
	{ 
		RecalculateViewport(windowInfo);
		m_sampleCount = 0;
		m_hasChanged = false;
	}

	++m_sampleCount;

#if Multithreading
	std::for_each(std::execution::par, verticalIter.begin(), verticalIter.end(),
		[this](uint32_t j)
		{
			std::for_each(horizontalIter.begin(), horizontalIter.end(),
			[this, j](uint32_t i)
				{
					color pixelColor = { 0,0,0 };
					Ray ray = GetRay(i, j);
					color newColor = RayColor(ray, m_maxBounce, *m_currentWorld);

					if (m_sampleCount > 1)
					{
						pixelColor = m_ImageBuffer[j * m_currentWindowInfo->width + i];
						const double x = (double)m_sampleCount;
						pixelColor = pixelColor * ((x - 1) / x) + newColor * (1.0 / x);
					}
					else
					{
						pixelColor = newColor;
					}

					m_ImageBuffer[j * m_currentWindowInfo->width + i] = pixelColor;
				});
		});
#else

	for (int j = 0; j < windowInfo.height; j++)
	{
		for (int i = 0; i < windowInfo.width; i++)
		{
			color pixelColor = { 0,0,0 };
			Ray ray = GetRay(i, j);
			color newColor = RayColor(ray, m_maxBounce, world);

			if (m_sampleCount > 1)
			{
				pixelColor = imageBuffer[j * windowInfo.width + i];
				const double x = (double)m_sampleCount;
				pixelColor = pixelColor * ((x - 1) / x) + newColor * (1.0 / x);
			}
			else
			{
				pixelColor = newColor;
			}

			imageBuffer[j * windowInfo.width + i] = pixelColor;
		}
	}
#endif

	std::cout << "Sample: " << m_sampleCount << std::endl;
}

double Camera::GetFocusDistanceOnClick(const HittableList& world) const
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	point3 pixelCenter = m_firstPixelLocation + m_pixelDeltaU * mouseX + m_pixelDeltaV * mouseY;
	point3 rayOrigin = m_position;
	vec3 rayDirection = pixelCenter - m_position;
	Ray ray(rayOrigin, rayDirection);
	HitRecord rec;

	if (world.Hit(ray, Interval(0.001, infinity), rec))
	{
		return (m_position - rec.objectCenter).length();
	}
	else
	{
		return m_focusDistance;
	}
}

Ray Camera::GetRay(int i, int j)
{
	point3 pixelCenter = m_firstPixelLocation + m_pixelDeltaU * i + m_pixelDeltaV * j;
	point3 pixelSample = pixelCenter + PixelSampleSquare();

	point3 rayOrigin;
	if (m_hasFocusBlur)
	{
		rayOrigin = (m_defocusAngle <= 0) ? m_position : DefocusDiskSample();
	}
	else
	{
		rayOrigin = m_position;
	}

	vec3 rayDirection = pixelSample - rayOrigin;

	return Ray(rayOrigin, rayDirection);
}

vec3 Camera::PixelSampleSquare()
{
	double deltaX = -0.5 + fastRandomDouble();
	double deltaY = -0.5 + fastRandomDouble();
	return deltaX * m_pixelDeltaU + deltaY * m_pixelDeltaV;
}


vec3 Camera::DefocusDiskSample()
{
	vec3 p = fastRandomInUnitDisk();
	return m_position + p[0] * m_defocusDistanceU + p[1] * m_defocusDistanceV;
}

color Camera::RayColor(const Ray& ray, int bounce, const Hittable& world)
{
	if (bounce <= 0)
	{
		return color(0, 0, 0);
	}

	HitRecord rec;
	if (world.Hit(ray, Interval(0.001, infinity), rec))
	{
		Ray scatterd;
		color attenuation;

		if (rec.material->Scatter(ray, rec, attenuation, scatterd))
		{
			return attenuation * RayColor(scatterd, bounce - 1, world);
		}

		return color(0, 0, 0);
	}

	// Color background with gradient if not hit.
	// viewport height range 0..1
	vec3 unitdirection = unitVector(ray.direction());
	double a = 0.5 * (unitdirection.y() + 1.0);
	color startcolor = { 0.5, 0.15, 0.0 };
	color endcolor = { 0.5, 0.5, 0.5 };
	color lerpedcolor = startcolor * (1 - a) + endcolor * a;
	return lerpedcolor;
}