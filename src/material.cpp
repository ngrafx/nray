#include "material.h"

#include "primitive.h"


Float Schlick(Float cosine, Float ref_idx) {
    auto r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}


bool LambertianMaterial::Scatter( const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered ) const  {
    Vec3 scatter_direction = rec.normal + RandomUnitVector<Float>();
    scattered = Ray(rec.p, scatter_direction, RayType::Diffuse);
    attenuation = _albedo;
    // attenuation = Vec3(1, 0, 1);
    return true;
}


bool DielectricMaterial::Scatter( const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered) const {
    attenuation = _albedo;
    Float etai_over_etat = (rec.front_face) ? (1.0 / _ref_idx) : (_ref_idx);

    Vec3 unit_direction = Normalize(r_in.Direction());
    auto cos_theta = Min( Dot(-unit_direction, rec.normal), 1.0);
    Float sin_theta = sqrt(1.0 - cos_theta*cos_theta);
    if (etai_over_etat * sin_theta > 1.0 ) {
        Vec3 reflected = Reflect(unit_direction, rec.normal);
        scattered = Ray(rec.p, reflected, RayType::Reflect);
        return true;
    }

    Float reflect_prob = Schlick(cos_theta, etai_over_etat);
    if (Rng::Rand01() < reflect_prob)
    {
        Vec3 reflected = Reflect(unit_direction, rec.normal);
        scattered = Ray(rec.p, reflected, RayType::Reflect);
        return true;
    }

    Vec3 refracted = Refract(unit_direction, rec.normal, etai_over_etat);
    scattered = Ray(rec.p, refracted, RayType::Refract);
    return true;
}


bool MetalMaterial::Scatter( const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered ) const  {
    Vec3 reflected = Reflect(Normalize(r_in.Direction()), rec.normal);
    scattered = Ray(rec.p, reflected + _fuzz*RandomVectorInUnitSphere<Float>(), RayType::Reflect);
    attenuation = _albedo;
    return (Dot(scattered.Direction(), rec.normal) > 0);
}