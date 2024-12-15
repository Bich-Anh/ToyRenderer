#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "hittable.h"

class material
{
 public:
  virtual ~material() = default;

  virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation,
                       ray& scattered) const = 0;
};

class lambertian : public material
{
 private:
  color albedo;

 public:
  lambertian(const color& albedo) : albedo(albedo) {}
  bool scatter(const ray& r_in, const hit_record& rec, color& attenuation,
               ray& scattered) const override
  {
    (void) r_in;  // Unused

    vec3 scatter_direction = rec.normal + random_unit_vector();
    if (scatter_direction.near_zero())
    {
      scatter_direction = rec.normal;
    }

    scattered = ray(rec.p, scatter_direction, r_in.time());
    attenuation = albedo;

    return true;
  }
};

class metal : public material
{
 private:
  color albedo;
  double fuzz;

 public:
  metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz) {}
  bool scatter(const ray& r_in, const hit_record& rec, color& attenuation,
               ray& scattered) const override
  {
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    reflected = unit_vector(reflected) + random_unit_vector() * fuzz;
    scattered = ray(rec.p, reflected, r_in.time());
    attenuation = albedo;

    return dot(scattered.direction(), rec.normal) > 0;
  }
};

class dielectric : public material
{
 private:
  double refraction_index;

  static double reflectance(double cosine, double refraction_index)
  {
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }

 public:
  dielectric(double refraction_index) : refraction_index(refraction_index) {}

  bool scatter(const ray& r_in, const hit_record& rec, color& attenuation,
               ray& scattered) const override
  {
    attenuation = color(1.0, 1.0, 1.0);  /// No attenuation

    double ri = rec.front_face
                    ? (1.0 / refraction_index)
                    : refraction_index;  // We consider eta_t to be 1 for air
    auto unit_direction = unit_vector(r_in.direction());

    auto cos_theta = std::fmin(1.0, dot(-unit_direction, rec.normal));
    auto sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
    bool cannot_refract = ri * sin_theta > 1.0;
    vec3 direction;

    if (cannot_refract || reflectance(cos_theta, ri) > random_double())
    {
      direction = reflect(unit_direction, rec.normal);
    }
    else
    {
      direction = refract(unit_direction, rec.normal, ri);
    }

    scattered = ray(rec.p, direction, r_in.time());

    return true;
  }
};

#endif