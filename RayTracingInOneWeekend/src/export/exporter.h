#pragma once

#include <string>

#include "../tools/color.h"
#include "../tools/windowinfo.h"

enum class ExportFormat 
{
	PNG,
	JPG,
	ELEMENT_COUNT
};

struct ExportInfo 
{
	ExportFormat format;
	struct SDL_Surface* imageSurface;
	std::string filePath;
	std::string fileName;
	int quality; 
};

class Exporter
{
public:
	Exporter() {};
	void ExportImage(ExportInfo Info);
private:
};