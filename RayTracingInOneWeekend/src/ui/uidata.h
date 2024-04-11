#pragma once

#include "../tools/vec3.h"

struct criticalData
{
	// Tracer Config
	int rayBounces = 10;
	double backgroundBrightness = 0.1;
	double hfov = 60.0;
	double defocusAngle = 1.5;

	// Camera
	vec3 camPosition{ -1, -1, -1 };
	vec3 camRotation{ -1, -1, -1 };
};

struct nonCriticalData
{
	// Input
	bool lockInput = false;

	// Post Process 
	bool bloom = false;

	// Analytics
	int sample = 0;
};

struct UIData
{
	// critical - data that, if changed, 
	// triggers a restart of the frame calculation.
	criticalData critical;
	nonCriticalData nonCritical;
};
