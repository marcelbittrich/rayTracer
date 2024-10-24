#pragma once

#include <memory>
#include <vector>

#include "input/input.h"
#include "input/mover.h"
#include "tools/rtweekend.h"
#include "objects/hittablelist.h"
#include "tools/windowInfo.h"
#include "tools/color.h"

#include "ui/uidata.h"

struct DebugInfo 
{
	int nodeHitChecks = 0;
	int primitiveHitChecks = 0;
};

class Camera
{
public:
	Camera(const WindowInfo& windowInfo);

	void HandleInput(const Input& input, double deltaTime);
	void Update(const Scene& world, color* imageBuffer, const WindowInfo& windowInfo);

	void GetDataForUI(UIData& uiData) const;
	int GetSampleNumber() const { return m_sampleCount; }
	point3 GetPosition() const { return m_position; }
	vec3 GetRotation() const{ return m_rotation; }
	
	void SetDataFromUI(const UIData& uiData);
	void SetHasChanged(bool value) { m_hasChanged = value; }

private:
	bool   m_hasFocusBlur  = true;
	const bool   m_showDebugNodesVisited = false;
	const bool   m_showDebugTriesTested = false;
	bool   m_inDebugMode = m_showDebugNodesVisited || m_showDebugTriesTested;
	int    m_maxBounce     = 10;
	double m_hfov          = 60.0;
	double m_focusDistance = 5.0;
	double m_defocusAngle  = 1.5;
	double m_backgroundBrightness = 0.1;
	vec3   m_defocusDistanceU;
	vec3   m_defocusDistanceV;
	vec3   m_pixelDeltaU;
	vec3   m_pixelDeltaV;
	point3 m_firstPixelLocation;
	double m_viewportHeight;
	double m_viewportWidth;
	point3 m_position;
	vec3   m_rotation = { 0,0,0 };
	vec3   m_viewDirection = {0,0,-1};

	int      m_sampleCount = 0;
	bool     m_hasChanged = false;
	bool     m_setFocusToMouse = false;

	double m_movementSpeed = 1.0;
	double m_rotatingSpeed = 1.0;
	double m_speedMultiplier = 2.0;
	Mover  m_mover = {
		m_movementSpeed,
		m_rotatingSpeed,
		m_speedMultiplier
	};

	std::vector<uint32_t> horizontalIter, verticalIter;
	const Scene* m_currentWorld = nullptr;
	const WindowInfo*	m_currentWindowInfo = nullptr;
	color* m_ImageBuffer;

	double GetFocusDistanceOnClick(const Scene& world) const;
	void SetPixelColor(int x , int y);
	void RecalculateViewport(const WindowInfo& windowInfo);
	Ray GetRay(int i, int j);
	vec3 PixelSampleSquare();
	vec3 DefocusDiskSample();
	color RayColor(const Ray& ray, int maxBounce, const Scene& world, DebugInfo& debugInfo);

	color DebugColor(int nodeHits, int primitiveHits);
};