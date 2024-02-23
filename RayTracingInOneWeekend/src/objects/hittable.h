#pragma once

#include "../tools/ray.h"

struct HitRecord
{
	point3 p;
	vec3 normal;
	double t;
};

class Hittable
{
public:
	virtual ~Hittable() = default;
	virtual bool Hit(const Ray& ray, double rayTmin, double rayTmax, HitRecord& rec) const = 0;

};
