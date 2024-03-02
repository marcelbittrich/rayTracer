#pragma once

#include "hittable.h"
#include "sphere.h"

#include <memory>
#include <vector>

using std::shared_ptr;

class HittableList : public Hittable
{
public:
	std::vector<shared_ptr<Hittable>> objects;
	std::vector<Sphere> m_spheres;

	HittableList() {}
	HittableList(shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<Hittable> object) { objects.push_back(object); }

	void addSphere(Sphere sphereToAdd) { m_spheres.push_back(sphereToAdd); }

	bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override
	{
		HitRecord tempRec;
		bool hitAnything = false;
		double closestSoFar = rayT.max;

		// Old approach with list of pointers
		//for (const shared_ptr<Hittable>& object : objects)
		//{
		//	if (object->Hit(ray, Interval(rayT.min, closestSoFar), tempRec))
		//	{
		//		hitAnything = true;
		//		closestSoFar = tempRec.t;
		//		rec = tempRec;
		//	}
		//}

		for (const Sphere& sphere : m_spheres)
		{
			if (sphere.Hit(ray, Interval(rayT.min, closestSoFar), tempRec))
			{
				hitAnything = true;
				closestSoFar = tempRec.t;
				rec = tempRec;
			}
		}

		return hitAnything;
	}
};