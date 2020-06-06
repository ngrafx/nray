#pragma once

#include "nray.h"
#include "geometry.h"


Float Schlick(Float cosine, Float ref_idx) {
    auto r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}


// Base Material class that every material needs to inherit
class Material  {
    public:
        virtual bool Scatter(
            const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered
        ) const = 0;
};


class LambertianMaterial : public Material {
    public:
        LambertianMaterial(const Color& albedo) : _albedo(albedo) {}

        virtual bool Scatter(
            const Ray& r_in, const Intersection& rec, Vec3& attenuation, Ray& scattered
        ) const;

    private:
        Color _albedo;
};


class DielectricMaterial : public Material {
    public:
        DielectricMaterial(Float refractive_index) : _ref_idx(refractive_index) {}

        virtual bool Scatter(
            const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered
        ) const;

    private:
        Float _ref_idx;
};


class MetalMaterial : public Material {
    public:
        MetalMaterial(const Color& albedo, Float fuzziness) : _albedo(albedo), _fuzz(fuzziness < 1 ? fuzziness : 1) {}

        virtual bool Scatter(
            const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered
        ) const;

    private:
        Color _albedo;
        Float _fuzz;
};