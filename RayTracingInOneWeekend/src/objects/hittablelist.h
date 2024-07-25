#pragma once

#include "bvh.h"
#include "hittable.h"
#include "sphere.h"
#include "triangle.h"
#include "diamond.h"
#include "../tools/aabb.h"

#include <vector>

class Scene
{
public:
	
	Scene() {};

	void AddSphere(Sphere sphereToAdd) 
	{ 
		m_spheres.push_back(sphereToAdd);
		ResetPointersToAllObjects();
	}

	void AddTriangle(Triangle triangleToAdd)
	{
		m_triangles.push_back(triangleToAdd);
		ResetPointersToAllObjects();
	}

	void AddDiamond(Diamond diamondToAdd)
	{
		m_diamonds.push_back(diamondToAdd);
		ResetPointersToAllObjects();
	}

	void ResetPointersToAllObjects()
	{
		m_objects.clear();
		for (int i = 0; i < m_spheres.size(); i++) 
		{
			m_objects.push_back(&m_spheres[i]);
		}
		for (int i = 0; i < m_triangles.size(); i++)
		{
			m_objects.push_back(&m_triangles[i]);
		}
		for (int i = 0; i < m_diamonds.size(); i++)
		{
			m_objects.push_back(&m_diamonds[i]);
		}
	}

	void SetupBVH()
	{
		ResetPointersToAllObjects();
		m_rootNode = new BVH_Node(this);
		m_hasBVH = true;
	}

	bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const
	{
		HitRecord tempRec;
		bool hitAnything = false;
		double closestSoFar = rayT.max;

		if (m_hasBVH)
		{
			if (m_rootNode->Hit(ray, Interval(rayT.min, closestSoFar), tempRec))
			{
				hitAnything = true;
				closestSoFar = tempRec.t;
				rec = tempRec;
			}
		}
		else
		{
			for (const Hittable* object : m_objects)
			{
				if (object->Hit(ray, Interval(rayT.min, closestSoFar), tempRec))
				{
					hitAnything = true;
					closestSoFar = tempRec.t;
					rec = tempRec;
				}
			}
		}

		return hitAnything;
	}

	std::vector<Hittable*>& GetObjectPointers() 
	{
		ResetPointersToAllObjects();
		return m_objects;
	}
public:
	BVH_Node* m_rootNode = nullptr;
private:
	bool m_hasBVH = false;
	std::vector<Sphere> m_spheres;
	std::vector<Triangle> m_triangles;
	std::vector<Diamond> m_diamonds;
	std::vector<Hittable*> m_objects;
};