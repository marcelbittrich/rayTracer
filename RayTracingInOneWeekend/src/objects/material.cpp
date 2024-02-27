#include "material.h"

#include "hittable.h"

bool Lambertian::Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered, uint32_t& seed) const
{
    vec3 scatterDirection = rec.normal + fastRandomUnitVector(seed);

    // Catch degenerate scatter direction
    if (scatterDirection.nearZero())
        scatterDirection = rec.normal;

    scattered = Ray(rec.p, scatterDirection);
    attenuation = m_albedo;

    return true;
}

bool Metal::Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered, uint32_t& seed) const
{
    vec3 reflected = reflect(ray.direction(), rec.normal);
    scattered = Ray(rec.p, reflected);
    attenuation = m_albedo;
    return true;
}
