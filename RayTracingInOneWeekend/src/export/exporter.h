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
	struct SDL_Surface* imageSurface;
	std::string filePath;
	std::string fileName;
	ExportFormat format;
	int quality; 
};

class Exporter
{
public:
	Exporter() {};
	void ExportImage(ExportInfo Info);
private:
};

static void ExoportNewStuff() 
{
	std::cout << "New Stuff" << std::endl;
}