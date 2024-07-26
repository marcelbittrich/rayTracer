#include "import.h"

namespace ObjectImport 
{
    namespace 
    {
        int parseOBJIndex(std::string& OBJIndexString, const std::string& characterToParseUntil)
        {
            std::size_t index = OBJIndexString.find_first_of(characterToParseUntil);
            if (index != std::string::npos)
            {
                OBJIndexString.erase(OBJIndexString.begin() + index, OBJIndexString.end());
                return std::stoi(OBJIndexString) - 1;
            }
            else
            {
                return std::stoi(OBJIndexString) - 1;
            }
        }
    }

    ObjectBuffer readOBJ(const char* filePath)
    {
        ObjectBuffer buffers;

        std::ifstream myfile(filePath);

        if (!myfile.is_open()) 
        {
            std::cout << "failed to open file from path: " << filePath << std::endl;
            return buffers;
        }

        while (!myfile.eof())
        {
            std::string line;
            getline(myfile, line);
            std::istringstream iss(line);

            if (line[0] == 'v' && line[1] == ' ')
            {
                std::string v, valueX, valueY, valueZ;
                iss >> v >> valueX >> valueY >> valueZ;
                const double x = std::stod(valueX);
                const double y = std::stod(valueY);
                const double z = std::stod(valueZ);
                vec3 point = { x,y,z };
                buffers.vertexBuffer.push_back(point);
            }

            if (line[0] == 'f')
            {
                std::string f, index1, index2, index3;

                iss >> f >> index1 >> index2 >> index3;

                const double x = (double)parseOBJIndex(index1, "/");
                const double y = (double)parseOBJIndex(index2, "/");
                const double z = (double)parseOBJIndex(index3, "/");
                vec3 point = { x,y,z };
                buffers.indexBuffer.push_back(point);
            }
        }

        return buffers;
    }
}
