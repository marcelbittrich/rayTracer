#pragma once

#include "hittable.h"
#include "sphere.h"
#include "../tools/aabb.h"

#include <memory>
#include <vector>

using std::shared_ptr;

class HittableList : public Hittable
{
public:
	std::vector<shared_ptr<Hittable>> objects;
	std::vector<Sphere> m_spheres;

	HittableList() {}
	HittableList(shared_ptr<Hittable> object) { Add(object); }

	void Clear() { objects.clear(); }
	void Add(shared_ptr<Hittable> object) 
	{ 
		objects.push_back(object); 
		bbox = AABB(bbox, object->BoundingBox());
	}

	void AddSphere(Sphere sphereToAdd) 
	{ 
		m_spheres.push_back(sphereToAdd);
		bbox = AABB(bbox, sphereToAdd.BoundingBox());
	}

	bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override
	{
		HitRecord tempRec;
		bool hitAnything = false;
		double closestSoFar = rayT.max;

		for (const shared_ptr<Hittable>& object : objects)
		{
			if (object->Hit(ray, Interval(rayT.min, closestSoFar), tempRec))
			{
				hitAnything = true;
				closestSoFar = tempRec.t;
				rec = tempRec;
			}
		}
		return hitAnything;
	}

	AABB BoundingBox() const override { return bbox; }

private:
	AABB bbox;
};