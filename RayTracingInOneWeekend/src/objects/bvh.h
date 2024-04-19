#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "../tools/aabb.h"
#include "../tools/rtweekend.h"
#include "hittable.h"
#include "hittablelist.h"

class BVH_Node : public Hittable
{
public:
	BVH_Node(HittableList list) : BVH_Node(list.objects, 0, list.objects.size()) {}

	BVH_Node(std::vector<shared_ptr<Hittable>>& objects, size_t start, size_t end)
	{
		bbox = AABB::empty;

		for (size_t objectIndex = start; objectIndex < end; objectIndex++)
		{
			bbox = AABB(bbox, objects[objectIndex]->BoundingBox());
		}

		int axis = bbox.LogestAxis();

		auto comperator = (axis == 0) ? BoxXCompare
			: (axis == 1) ? BoxYCompare
			: BoxZCompare;

		size_t objectSpan = end - start;

		if (objectSpan == 1)
		{
			left = right = objects[start];
		}
		else if (objectSpan == 2)
		{
			left = objects[start];
			right = objects[start+1];
		}
		else
		{
			std::sort(objects.begin() + start, objects.begin() + end, comperator);

			size_t mid = start + objectSpan / 2;
			left = make_shared<BVH_Node>(objects, start, mid);
			right = make_shared<BVH_Node>(objects, mid, end);
		}
	}

	bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override 
	{
		if (!bbox.Hit(ray, rayT))
		{
			return false;
		}

		bool hitLeft = left->Hit(ray, rayT, rec);
		bool hitRight = right->Hit(ray, Interval(rayT.min, hitLeft ? rec.t : rayT.max), rec);

		return hitLeft || hitRight;
	}

	int GetLeafCount() const
	{
		int sum = 0;
		if (auto node = std::dynamic_pointer_cast<BVH_Node>(left))
			sum += node->GetLeafCount();
		else
			sum++;

		if (auto node = std::dynamic_pointer_cast<BVH_Node>(right))
			sum += node->GetLeafCount();
		else	
			sum++;

		return sum;
	}

	AABB BoundingBox() const override { return bbox; }

	shared_ptr<Hittable> left;
	shared_ptr<Hittable> right;
private:
	AABB bbox;

	static bool BoxCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axisIndex)
	{
		auto aAxisInterval = a->BoundingBox().AxisInterval(axisIndex);
		auto bAxisInterval = b->BoundingBox().AxisInterval(axisIndex);
		return aAxisInterval.min < bAxisInterval.min;
	}

	static bool BoxXCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
	{
		return BoxCompare(a, b, 0);
	}
	static bool BoxYCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
	{
		return BoxCompare(a, b, 1);
	}
	static bool BoxZCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
	{
		return BoxCompare(a, b, 2);
	}
};