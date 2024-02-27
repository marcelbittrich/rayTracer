#pragma once

#include "hittable.h"
#include "../tools/vec3.h"

class Sphere : public Hittable
{
public:
	Sphere(point3 center, double radius, shared_ptr<Material> material) 
		: m_center(center), m_radius(radius), m_material(material){}

	bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override
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
		
		// If root is out of interval try with positive sqrtd.
		if (!rayT.Surrounds(root))
		{
			root = (-half_b + sqrtd) / a;
			// If this is also out of interval return.
			if (!rayT.Surrounds(root))
			{
				return false;
			}
		}

		rec.t = root;
		rec.p = ray.at(rec.t);
		vec3 outwardNormal = (rec.p - m_center) / m_radius;
		rec.SetFaceNormal(ray, outwardNormal);
		rec.material = m_material;

		return true;
	}

private:
	point3 m_center;
	double m_radius;
	shared_ptr<Material> m_material;
};
