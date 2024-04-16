#pragma once

#include "interval.h"
#include "ray.h"
#include "vec3.h"

class AABB 
{
public:
	Interval x, y, z;

	// Default AABB is empty, since Intervals are empty by default.
	AABB() {} 

	AABB(const Interval& x, const Interval& y, const Interval& z)
		: x(x), y(y), z(z) {}

	// Construct with points with extrem values.
	AABB(const point3& a, const point3& b)
	{
		x = (a[0] < b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
		y = (a[1] < b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
		z = (a[2] < b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
	}

	AABB(const AABB& box0, const AABB& box1)
	{
		x = Interval(box0.x, box1.x);
		y = Interval(box0.y, box1.y);
		z = Interval(box0.z, box1.z);
	}

	const Interval& AxisInterval(int n) const 
	{
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	bool Hit(const Ray& ray, Interval rayT) const 
	{
		const point3& rayOrigin = ray.Origin();
		const vec3& rayDirection = ray.Direction();
	
		for (int axis = 0; axis < 3; axis++)
		{
			const Interval& ax = AxisInterval(axis);
			const double adinv = 1.0 / rayDirection[axis];

			double t0 = (ax.min - rayOrigin[axis]) * adinv;
			double t1 = (ax.max - rayOrigin[axis]) * adinv;

			if (t0 < t1)
			{
				if (t0 > rayT.min) rayT.min = t0;
				if (t1 < rayT.max) rayT.max = t1;
			}
			else
			{
				if (t1 > rayT.min) rayT.min = t1;
				if (t0 < rayT.max) rayT.max = t0;
			}

			if (rayT.max <= rayT.min)
				return false;
		}
		return true;
	}

	int LogestAxis() const 
	{
		if (x.Size() > y.Size())
			return x.Size() > z.Size() ? 0 : 2;
		else
			return y.Size() > z.Size() ? 1 : 2;
	}

	static const AABB empty, universe;
};

