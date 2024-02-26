#pragma once

struct WindowInfo
{
	double aspectRatio;
	int width;
	int height;
	//int samplesPerPixel;
	bool hasChanged = false;
};