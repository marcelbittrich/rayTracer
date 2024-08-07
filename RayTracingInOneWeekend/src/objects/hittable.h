#pragma once

#include "../tools/ray.h"
#include "../tools/interval.h"
#include "../tools/aabb.h"

class Material;

struct HitRecord
{
	point3 p;
	vec3 normal;
	vec3 objectCenter;
	shared_ptr<Material> material;
	double t;
	bool frontFace;

	int nodeHitChecks = 0;
	int primitiveHitChecks = 0;

	void SetFaceNormal(const Ray& ray, const vec3& outwardNormal)
	{
		// Sets hit record normal vector.
		// NOTE: the parameter outward_normal is assumend to have unit length.
	
		frontFace = dot(ray.Direction(), outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

class Hittable
{
public:
	virtual ~Hittable() = default;
	virtual bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const = 0;
	virtual AABB BoundingBox() const = 0;
};
