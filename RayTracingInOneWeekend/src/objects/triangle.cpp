#include "triangle.h"

bool Triangle::Hit(const Ray& ray, Interval rayT, HitRecord& rec) const
{
    constexpr double epsilon = std::numeric_limits<double>::epsilon();

    const vec3 edge1 = m_points.at(1) - m_points.at(0);
    const vec3 edge2 = m_points.at(2) - m_points.at(0);
    const vec3 rayCrossEdge2 = cross(ray.Direction(), edge2);
    const double det = dot(edge1, rayCrossEdge2);

    if (det > -epsilon && det < epsilon)
        return false;

    const double invDet = 1.0 / det;
    const vec3 s = ray.Origin() - m_points.at(0);
    const double u = invDet * dot(s, rayCrossEdge2);

    if (u < 0 || u > 1)
        return false;

    const vec3 sCrossEdge1 = cross(s, edge1);
    const double v = invDet * dot(ray.Direction(), sCrossEdge1);

    if (v < 0 || u + v > 1)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    double t = invDet * dot(edge2, sCrossEdge1);

    if (t > epsilon) // ray intersection
    {
        rec.t = t;
        rec.p = ray.At(rec.t);
        rec.objectCenter = (m_points[0] + m_points[1] + m_points[2]) / 3;
        const vec3 outwardNormal = unitVector(cross(edge1, edge2));
        rec.SetFaceNormal(ray, outwardNormal);
        rec.material = m_material;
        return  true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}
