#pragma once

#include <cmath>
#include <cstdlib>
#include <memory>

// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degreesToRadians(double degrees)
{
	return degrees * pi / 180.0;
}

inline uint32_t PCGHash(uint32_t input)
{
	uint32_t state = input * 747796405u + 2891336453u;
	uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

inline double fastRandomDouble()
{
	thread_local static uint32_t seed = 0;
	seed = PCGHash(seed);
	return (double)seed / UINT32_MAX;
}

inline double fastRandomDouble(double min, double max)
{
	return min + (max - min) * fastRandomDouble();
}

inline double randomDouble()
{
	// Returns a random real in [0,1).
	return (double)rand() / RAND_MAX;
}

inline double randomDouble(double min, double max)
{
	// Returns a random real in [min,max).
	return min + (max - min) * randomDouble();
}

inline int randomInt(int min, int max)
{
	return (int)fastRandomDouble(min, max + 1);
}