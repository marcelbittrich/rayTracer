#include "exporter.h"

#include "SDL_image.h"

void Exporter::ExportImage(ExportInfo info)
{
	if (info.format == ExportFormat::JPG)
	{
		std::cout << "Exporter Export JPG to " << info.filePath  << " with quality: "  << info.quality << std::endl;
		info.filePath += info.fileName + ".jpg";
		IMG_SaveJPG(info.imageSurface, info.filePath.c_str(), info.quality);
	}
	else if (info.format == ExportFormat::PNG)
	{
		std::cout << "Exporter Export PNG to " << info.filePath << std::endl;
		info.filePath += info.fileName + ".png";
		IMG_SavePNG(info.imageSurface, info.filePath.c_str());
	}
}
