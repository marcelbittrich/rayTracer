#pragma once

#include "../tools/ray.h"

struct HitRecord
{
	point3 p;
	vec3 normal;
	double t;
	bool frontFace;

	void SetFaceNormal(const Ray& ray, const vec3& outwardNormal)
	{
		// Sets hit record normal vector.
		// NOTE: the parameter outward_normal is assumend to have unit length.
	
		frontFace = dot(ray.direction(), outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

class Hittable
{
public:
	virtual ~Hittable() = default;
	virtual bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const = 0;

};
