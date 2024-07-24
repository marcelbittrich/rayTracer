#pragma once

#include "array"
#include "hittable.h"
#include "../tools/vec3.h"

class Triangle : public Hittable
{
public:
	Triangle(std::array<vec3, 3> points, shared_ptr<Material> material) : m_points(points), m_material(material)
	{
		const AABB bboxAB = AABB(points[0], points[1]);
		const AABB bboxAC = AABB(points[0], points[2]);
		bbox = AABB(bboxAB, bboxAC);
	};

	bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;

	AABB BoundingBox() const override { return bbox; };

	shared_ptr<Material> m_material;
private:
	std::array<vec3, 3> m_points;
	AABB bbox;
};

