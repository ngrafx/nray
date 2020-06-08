#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "nray.h"
#include "geometry.h"


Float Schlick(Float cosine, Float ref_idx);


// Base Material class that every material needs to inherit
class Material  {
    public:
        virtual bool Scatter(
            const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered
        ) const = 0;
};


// Lambertian Material
class LambertianMaterial : public Material {
    public:
        LambertianMaterial(const Color& albedo) : _albedo(albedo) {}

        virtual bool Scatter(
            const Ray& r_in, const Intersection& rec, Vec3& attenuation, Ray& scattered
        ) const;

    private:
        Color _albedo;
};

// Dielectric Material
class DielectricMaterial : public Material {
    public:
        DielectricMaterial(Float refractive_index) : _ref_idx(refractive_index) {}

        virtual bool Scatter(
            const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered
        ) const;

    private:
        Float _ref_idx;
};

// Metal Material
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

#endif