#include "material.h"

#include "hittable.h"

bool Lambertian::Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered) const
{
    vec3 scatterDirection = rec.normal + fastRandomUnitVector();

    // Catch degenerate scatter direction
    if (scatterDirection.nearZero())
        scatterDirection = rec.normal;

    scattered = Ray(rec.p, scatterDirection);
    attenuation = m_albedo;

    return true;
}

bool Metal::Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered) const
{
    vec3 reflected = reflect(ray.direction(), rec.normal);
    scattered = Ray(rec.p, reflected + m_fuzz * fastRandomUnitVector());
    attenuation = m_albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
}

bool Dielectric::Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered) const
{
    attenuation = m_albedo;
    double refractionRatio = rec.frontFace ? (1.0 / m_refractionIndex) : m_refractionIndex;

    vec3 unitDirection = unitVector(ray.direction());

    double cosTheta = fmin(dot(-unitDirection, rec.normal), 1.0);
    double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    bool cannotRefract = (refractionRatio * sinTheta) > 1.0;
    vec3 direction;

    if (cannotRefract || (reflectance(cosTheta, refractionRatio) > fastRandomDouble()))
    {
        direction = reflect(unitDirection, rec.normal);
    }
    else 
    {
        direction = refract(unitDirection, rec.normal, refractionRatio);
    }

    scattered = Ray(rec.p, direction);
    return true;
}

double Dielectric::reflectance(double cosine, double refractionIndex) const
{
    // Schlick approximation for reflectance
    double r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}
