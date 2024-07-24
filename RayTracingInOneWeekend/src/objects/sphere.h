#pragma once

#include "hittable.h"
#include "../tools/vec3.h"

class Sphere : public Hittable
{
public:
	Sphere(point3 center, double radius, shared_ptr<Material> material) 
		: m_center(center), m_radius(fmax(0.0,radius)), m_material(material)
	{
		vec3 radiusVector(radius, radius, radius);
		bbox = AABB(center - radiusVector, center + radiusVector);
	}

	bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override
	{
		const vec3 oc = ray.Origin() - m_center;
		const double a = ray.Direction().length_squared();
		const double half_b = dot(oc, ray.Direction());
		const double c = oc.length_squared() - m_radius * m_radius;
		const double discriminant = half_b * half_b - a * c;

		if (discriminant < 0)
		{
			return false;
		}

		const double sqrtd = sqrt(discriminant);
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
		rec.p = ray.At(rec.t);
		rec.objectCenter = m_center;
		const vec3 outwardNormal = (rec.p - m_center) / m_radius;
		rec.SetFaceNormal(ray, outwardNormal);
		rec.material = m_material;

		return true;
	}

	AABB BoundingBox() const override { return bbox; };

	point3 m_center;
	double m_radius;
	shared_ptr<Material> m_material;
private:
	AABB bbox;
};
