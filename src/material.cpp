#include "material.h"

#include "primitive.h"

bool DielectricMaterial::Scatter( const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered) const {
    attenuation = Color(1.0, 1.0, 1.0);
    Float etai_over_etat = (rec.front_face) ? (1.0 / _ref_idx) : (_ref_idx);

    Vec3 unit_direction = Normalize(r_in.Direction());
    Float cos_theta = min(Dot(-unit_direction, rec.normal), 1.0);
    Float sin_theta = sqrt(1.0 - cos_theta*cos_theta);
    if (etai_over_etat * sin_theta > 1.0 ) {
        Vec3 reflected = Reflect(unit_direction, rec.normal);
        scattered = Ray(rec.p, reflected);
        return true;
    }

    Float reflect_prob = Schlick(cos_theta, etai_over_etat);
    if (random_double() < reflect_prob)
    {
        Vec3 reflected = Reflect(unit_direction, rec.normal);
        scattered = Ray(rec.p, reflected);
        return true;
    }

    Vec3 refracted = Refract(unit_direction, rec.normal, etai_over_etat);
    scattered = Ray(rec.p, refracted);
    return true;
}