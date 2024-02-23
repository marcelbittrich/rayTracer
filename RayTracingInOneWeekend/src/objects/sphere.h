#pragma once

#include "hittable.h"
#include "../tools/vec3.h"

class Sphere : public Hittable
{
public:
	Sphere(point3 center, double radius) : m_center(center), m_radius(radius) {}

	bool Hit(const Ray& ray, double rayTmin, double rayTmax, HitRecord& rec) const override
	{
		vec3 oc = ray.origin() - m_center;
		double a = ray.direction().length_squared();
		double half_b = dot(oc, ray.direction());
		double c = oc.length_squared() - m_radius * m_radius;
		double discriminant = half_b * half_b - a * c;

		if (discriminant < 0)
		{
			return false;
		}

		double sqrtd = sqrt(discriminant);

		double root = (-half_b - sqrtd) / a;
		
		// If root is out of bounds try with positive sqrtd
		if (root <= rayTmin || rayTmax <= root)
		{
			root = (-half_b + sqrtd) / a;
			if (root <= rayTmin || rayTmax <= root)
			{
				return false;
			}
		}

		rec.t = root;
		rec.p = ray.at(rec.t);
		rec.normal = (rec.p - m_center) / m_radius;
	}

private:
	point3 m_center;
	double m_radius;
};
