#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <string>
#include "../tools/vec3.h"

namespace ObjectImport 
{
	struct ObjectBuffer 
	{
		std::vector<vec3> vertexBuffer;
		std::vector<vec3> indexBuffer;
	};

	ObjectBuffer readOBJ(const char* filePath);
}
