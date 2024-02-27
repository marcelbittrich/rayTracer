#pragma once

#include "../tools/rtweekend.h"
#include "../tools/ray.h"
#include "../tools/color.h"

struct HitRecord;

class Material
{
public:
	virtual ~Material() = default;

	virtual bool Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered, uint32_t& seed) const = 0;
};

class Lambertian : public Material
{
public:
	Lambertian(const color& albedo) : m_albedo(albedo) {}

	bool Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered, uint32_t& seed) const override;

private:
	color m_albedo;
};

class Metal : public Material
{
public:
	Metal(const color& albedo) : m_albedo(albedo) {}

	bool Scatter(const Ray& ray, const HitRecord& rec, color& attenuation, Ray& scattered, uint32_t& seed) const override;

private:
	color m_albedo;
};