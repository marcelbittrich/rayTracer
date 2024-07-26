#pragma once

#include <vector>
#include "hittable.h"
#include "triangle.h"
#include "../tools/vec3.h"
#include "../import/import.h"

class PolygonObject : public Hittable
{
public:
	PolygonObject(
		const ObjectImport::ObjectBuffer& buffers,
		point3 center,
		shared_ptr<Material> material
	)
		: m_center(center), m_material(material)
	{
		const std::vector<vec3> vertices = buffers.vertexBuffer;
		const std::vector<vec3> indices = buffers.indexBuffer;

		for (int i = 0; i < indices.size(); i++) {
			vec3 p1 = {
				vertices[(int)indices[i].x()].x(),
				vertices[(int)indices[i].x()].y(),
				vertices[(int)indices[i].x()].z()
			};
			vec3 p2 = {
				vertices[(int)indices[i].y()].x(),
				vertices[(int)indices[i].y()].y(),
				vertices[(int)indices[i].y()].z()
			};
			vec3 p3 = {
				vertices[(int)indices[i].z()].x(),
				vertices[(int)indices[i].z()].y(),
				vertices[(int)indices[i].z()].z()
			};

			p1 += center;
			p2 += center;
			p3 += center;

			m_triangles.push_back(Triangle({ p1,p2,p3 }, m_material));
		}

		bbox = AABB::empty;
		for (int i = 0; i < m_triangles.size(); i++)
		{
			bbox = AABB(bbox, m_triangles[i].BoundingBox());
		}

		std::cout << "PolygonGeometry tris count: " << m_triangles.size() << std::endl;
		std::cout << "With an AABB: " << bbox << std::endl;
		
	}

	bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override
	{
		rec.nodeHitChecks++;
		if (!bbox.Hit(ray, rayT))
		{
			return false;
		}

		rec.primitiveHitChecks++;
		HitRecord tempRec;
		tempRec.primitiveHitChecks = rec.primitiveHitChecks;
		tempRec.nodeHitChecks = rec.nodeHitChecks;
		bool hitAnything = false;
		double closestSoFar = rayT.max;

		for (const Triangle& tri : m_triangles)
		{		
			if (tri.Hit(ray, Interval(rayT.min, closestSoFar), tempRec))
			{
				hitAnything = true;
				closestSoFar = tempRec.t;
				tempRec.primitiveHitChecks += rec.primitiveHitChecks;
				tempRec.nodeHitChecks += rec.nodeHitChecks;
				rec = tempRec;
			}
		}

		rec.primitiveHitChecks = tempRec.primitiveHitChecks;
		rec.nodeHitChecks = tempRec.nodeHitChecks;

		if (hitAnything) {
			rec.objectCenter = m_center;
		}

		return hitAnything;
	}

	AABB BoundingBox() const override { return bbox; };

	point3 m_center;
	shared_ptr<Material> m_material;

private:
	std::vector<Triangle> m_triangles;
	AABB bbox;
};