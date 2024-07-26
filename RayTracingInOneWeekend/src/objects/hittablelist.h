#pragma once

#include "bvh.h"
#include "hittable.h"
#include "sphere.h"
#include "triangle.h"
#include "diamond.h"
#include "polygonobject.h"
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

	void AddPolygonObject(PolygonObject polygonObjectToAdd)
	{
		m_polygonObjects.push_back(polygonObjectToAdd);
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
		for (int i = 0; i < m_polygonObjects.size(); i++)
		{
			m_objects.push_back(&m_polygonObjects[i]);
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
		bool hitAnything = false;
		
		if (m_hasBVH)
		{
			hitAnything = m_rootNode->Hit(ray, rayT, rec);
		}
		else
		{
			HitRecord tempRec;
			tempRec.primitiveHitChecks = rec.primitiveHitChecks;
			tempRec.nodeHitChecks = rec.nodeHitChecks;
			double closestSoFar = rayT.max;

			for (const Hittable* object : m_objects)
			{
				if (object->Hit(ray, Interval(rayT.min, closestSoFar), tempRec))
				{
					hitAnything = true;
					closestSoFar = tempRec.t;
					rec = tempRec;
				}
			}

			rec.primitiveHitChecks = tempRec.primitiveHitChecks;
			rec.nodeHitChecks = tempRec.nodeHitChecks;
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
	std::vector<PolygonObject> m_polygonObjects;
	std::vector<Hittable*> m_objects;
};