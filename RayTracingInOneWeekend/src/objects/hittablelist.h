#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;

class HittableList : public Hittable
{
public:
	std::vector<shared_ptr<Hittable>> objects;

	HittableList() {}
	HittableList(shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<Hittable> object) { objects.push_back(object); }

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
};