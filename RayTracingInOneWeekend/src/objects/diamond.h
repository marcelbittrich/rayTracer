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
		const std::array<double, 18> vertices = {
			 left.x(),		left.y(),	left.z(),	// Left vertex
			 right.x(),		right.y(),  right.z(),  // Right vertex
			 up.x(),		up.y(),		up.z(),		// Top vertex
			 down.x(),		down.y(),	down.z(),	// Bottom vertex
			 front.x(),		front.y(),  front.z(),  // Front vertex
			 back.x(),		back.y(),	back.z(),   // Back vertex
		};

		// Index array
		const std::array<unsigned int, 24> indices = {
			0, 4, 2,  // Left, Front, Top
			1, 2, 4,  // Right, Top, Front
			1, 4, 3,  // Right, Front, Bottom
			0, 3, 4,  // Left, Bottom, Front
			0, 2, 5,  // Left, Top, Back
			1, 5, 2,  // Right, Back, Top
			1, 3, 5,  // Right, Bottom, Back
			0, 5, 3   // Left, Back, Bottom
		};

		for (int i = 0, j = 0; i < indices.size(); i += 3, j++) {
			vec3 p1 = { vertices[3 * indices[i]], vertices[3 * indices[i] + 1], vertices[3 * indices[i] + 2] };
			vec3 p2 = { vertices[3 * indices[i + 1]], vertices[3 * indices[i + 1] + 1], vertices[3 * indices[i + 1] + 2] };
			vec3 p3 = { vertices[3 * indices[i + 2]], vertices[3 * indices[i + 2] + 1], vertices[3 * indices[i + 2] + 2] };

			const std::array<vec3, 3> points = { p1, p2, p3 };
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
		HitRecord tempRec;
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