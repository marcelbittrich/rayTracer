#include "camera.h"

#include <algorithm>
#include <execution>

#include "SDL_mouse.h"
#include "SDL_stdinc.h"

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
	horizontalIter.clear();
	verticalIter.clear();
	horizontalIter.resize(windowInfo.width);
	verticalIter.resize(windowInfo.height);
	for (uint32_t i = 0; i < (uint32_t)windowInfo.width; i++)
		horizontalIter[i] = i;
	for (uint32_t i = 0; i < (uint32_t)windowInfo.height; i++)
		verticalIter[i] = i;
#endif

	// Calculation of the vieport local to camera
	double theta = degreesToRadians(m_hfov);
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

void Camera::Update(const Scene& world, color* imageBuffer, const WindowInfo& windowInfo)
{
	m_currentWorld = &world;
	m_currentWindowInfo = &windowInfo;
	m_ImageBuffer = imageBuffer;
	m_inDebugMode = m_showDebugNodesVisited || m_showDebugTriesTested;

	if (m_hasFocusBlur && m_setFocusToMouse)
	{
		double prevFocusDistance = m_focusDistance;
		m_focusDistance = GetFocusDistanceOnClick(world);
		m_hasChanged = prevFocusDistance != m_focusDistance;
		m_setFocusToMouse = false;
	}

	if (m_hasChanged || windowInfo.hasChanged)
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
					SetPixelColor(i, j);
				});
		});
#else

	for (int j = 0; j < windowInfo.height; j++)
	{
		for (int i = 0; i < windowInfo.width; i++)
		{
			SetPixelColor(i, j);
		}
	}
#endif
}

void Camera::GetDataForUI(UIData& uiData) const
{
	uiData.nonCritical.sample = m_sampleCount;

	uiData.critical.camPosition = m_position;
	uiData.critical.camRotation = m_rotation;
	uiData.critical.rayBounces = m_maxBounce;
	uiData.critical.backgroundBrightness = m_backgroundBrightness;
	uiData.critical.hfov = m_hfov;
	uiData.critical.defocusAngle = m_defocusAngle;
	uiData.critical.hasFocusBlur = m_hasFocusBlur;
	uiData.critical.focusDistance = m_focusDistance;
}

void Camera::SetDataFromUI(const UIData& uiData)
{
	m_maxBounce = uiData.critical.rayBounces;
	m_position = uiData.critical.camPosition;
	m_rotation = uiData.critical.camRotation;
	m_backgroundBrightness = uiData.critical.backgroundBrightness;
	m_hfov = uiData.critical.hfov;
	m_defocusAngle = uiData.critical.defocusAngle;
	m_hasFocusBlur = uiData.critical.hasFocusBlur;
	m_focusDistance = uiData.critical.focusDistance;
}

double Camera::GetFocusDistanceOnClick(const Scene& world) const
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
		double newfocusDistance = (m_position - rec.objectCenter).length();
		return newfocusDistance > 100.0 ? m_focusDistance : newfocusDistance;
	}
	else
	{
		return m_focusDistance;
	}
}

void Camera::SetPixelColor(int x, int y)
{
	color pixelColor = { 0,0,0 };
	Ray ray = GetRay(x, y);
	DebugInfo debugInfo;
	color newColor = RayColor(ray, m_maxBounce, *m_currentWorld, debugInfo);

	if (m_showDebugNodesVisited)
	{
		const int maxVisits = 200;
		color zeroVisitsColor = { 0,0,0.4 };
		color maxVisitsColor = { 0.4,0.4,0 };

		const double ratio = SDL_clamp((double)debugInfo.nodeHitChecks/maxVisits,0.0,1.0);
		newColor = zeroVisitsColor * (1 - ratio) + maxVisitsColor * ratio;
	}
	else if (m_showDebugTriesTested)
	{
		const int maxTested = 100;
		color zeroTestedColor = { 0,0,0.4 };
		color maxTestedColor = { 0.4,0.4,0 };

		const double ratio = SDL_clamp((double)debugInfo.primitiveHitChecks / maxTested, 0.0, 1.0);
		newColor = zeroTestedColor * (1 - ratio) + maxTestedColor * ratio;
	}

	if (m_sampleCount > 1)
	{
		pixelColor = m_ImageBuffer[y * m_currentWindowInfo->width + x];
		const double sc = (double)m_sampleCount;
		pixelColor = pixelColor * ((sc - 1) / sc) + newColor * (1.0 / sc);
	}
	else
	{
		pixelColor = newColor;
	}

	m_ImageBuffer[y * m_currentWindowInfo->width + x] = pixelColor;
}

Ray Camera::GetRay(int i, int j)
{
	point3 pixelCenter = m_firstPixelLocation + m_pixelDeltaU * i + m_pixelDeltaV * j;
	point3 pixelSample = pixelCenter + PixelSampleSquare();

	point3 rayOrigin;
	if (m_hasFocusBlur && !m_inDebugMode)
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

color Camera::RayColor(const Ray& ray, int bounce, const Scene& world, DebugInfo& debugInfo)
{
	if (bounce <= 0)
	{
		return color(0, 0, 0);
	}

	HitRecord rec;
	const bool hitWorld = world.Hit(ray, Interval(0.001, infinity), rec);
	debugInfo.nodeHitChecks += rec.nodeHitChecks;
	debugInfo.primitiveHitChecks += rec.primitiveHitChecks;

	if (!hitWorld)
	{
		// Color background with gradient if not hit.
		vec3 unitdirection = unitVector(ray.Direction());
		double a = 0.5 * (unitdirection.y() + 1.0);
		color startcolor = { 1.0, 0.3, 0.0 };
		color endcolor = { 1.0, 1.0, 1.0 };
		color lerpedcolor = startcolor * (1 - a) + endcolor * a;
		return lerpedcolor * m_backgroundBrightness;
	}

	Ray scatterd;
	color attenuation;
	color colorFromEmission = rec.material->Emitted();

	if (!rec.material->Scatter(ray, rec, attenuation, scatterd))
	{
		return colorFromEmission;
	}

	color colorFromScatter = attenuation * RayColor(scatterd, bounce - 1, world, debugInfo);

	return colorFromEmission + colorFromScatter;
}

color Camera::DebugColor(int nodeHits, int primitiveHits)
{
	const double intesityNodeHits = 1.0 - 1.0 / (nodeHits + 1);
	const double intesityPrimitiveHits = 1.0 - 1.0 / (primitiveHits + 1);
	return color(0.5, 0, 0) * intesityPrimitiveHits + color(0, 0.5, 0) * intesityNodeHits;
}

