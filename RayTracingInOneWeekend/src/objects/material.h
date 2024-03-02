#pragma once

#include "../tools/rtweekend.h"
#include "../tools/ray.h"
#include "../tools/color.h"

struct HitRecord;

class Material
{
public:
	virtual ~Material() = default;

	virtual bool Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered) const = 0;
};

class Lambertian : public Material
{
public:
	Lambertian(const color& albedo) : m_albedo(albedo) {}

	bool Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered) const override;

private:
	color m_albedo;
};

class Metal : public Material
{
public:
	Metal(const color& albedo, double fuzz) 
		: m_albedo(albedo), m_fuzz(fuzz < 1 ? fuzz : 1) {}

	bool Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered) const override;

private:
	color m_albedo;
	double m_fuzz;
};

class Dielectric : public Material
{
public:
	Dielectric(double refrectionIndex)
		: m_refractionIndex(refrectionIndex){}

	bool Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered) const override;

private:
	double m_refractionIndex;

	double reflectance(double cosine, double refractionIndex) const;
};