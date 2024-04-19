#pragma once

#include "../tools/vec3.h"
#include "../export/exporter.h"

struct criticalData
{
	// World
	bool hasWorldChanged = false;

	// Tracer Config
	bool   hasFocusBlur = true;
	int    rayBounces = 10;
	double backgroundBrightness = 0.1;
	double hfov = 60.0;
	double defocusAngle = 1.5;
	double focusDistance = 10.0;

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

	// Export Image
	bool exportImage = false;
	ExportFormat exportImageFormat = ExportFormat::PNG;
	int exportImageQuality = 100;
};

struct UIData
{
	// critical - data that, if changed, 
	// triggers a restart of the frame calculation.
	criticalData critical;
	nonCriticalData nonCritical;
};
