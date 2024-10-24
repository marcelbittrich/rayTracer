#pragma once

#include <array>
#include "hittable.h"
#include "triangle.h"
#include "../tools/vec3.h"

class Diamond : public Hittable 
{
public:
	Diamond(point3 center, double height, shared_ptr<Material> material)
		: m_center(center), m_height(height), m_material(material)
	{
		const vec3 up		= center + vec3(0, height/2 ,0);
		const vec3 down		= center - vec3(0, height / 2, 0);
		const vec3 left		= center - vec3(height / 2, 0, 0);
		const vec3 right	= center + vec3(height / 2, 0, 0);
		const vec3 front	= center - vec3(0, 0, height / 2);
		const vec3 back		= center + vec3(0, 0, height / 2);

		// Vertex array
		const std::array<vec3, 6> vertices = {
			 left,
			 right,
			 up, 
			 down,
			 front,
			 back,
		};

		// Index array
		const std::array<unsigned int, 24> indices = {
			2, 4, 0,  // Top, Front, Left
			4, 2, 1,  // Font, Top, Right
			3, 4, 1,  // Bottom, Front, Right
			4, 3, 0,  // Front, Bottom, Left
			5, 2, 0,  // Back, Top, Left
			2, 5, 1,  // Top, Back, Right
			5, 3, 1,  // Back, Bottom, Right
			3, 5, 0   // Bottom, Back, Left
		};

		for (int i = 0, j = 0; i < indices.size(); i += 3, j++) {
			vec3 p1 = { 
				vertices[indices[i]].x(), 
				vertices[indices[i]].y(), 
				vertices[indices[i]].z()
			};
			vec3 p2 = { 
				vertices[indices[i + 1]].x(), 
				vertices[indices[i + 1]].y(), 
				vertices[indices[i + 1]].z()
			};
			vec3 p3 = { 
				vertices[indices[i + 2]].x(),
				vertices[indices[i + 2]].y(),
				vertices[indices[i + 2]].z()
			};

			m_triangles.at(j) = Triangle({ p1,p2,p3 }, m_material);
		}

		bbox = AABB::empty;
		for (int i = 0; i < m_triangles.size(); i++) 
		{
			bbox = AABB(bbox, m_triangles[i].BoundingBox());
		}
	}

	bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override
	{
		rec.nodeHitChecks++;

		if (!bbox.Hit(ray, rayT))
		{
			return false;
		}

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
	double m_height;
	shared_ptr<Material> m_material;

private:
	std::array<Triangle, 8> m_triangles;
	AABB bbox;
};