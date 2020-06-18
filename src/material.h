#pragma once

#include "nray.h"
#include "geometry.h"

// Fresnel like function to compute a mask
// between reflections and refractions for
// Dielectric
Float Schlick(Float cosine, Float ref_idx);


// Base Material class that every material needs to inherit
// Materials describe how light rays interacts with a primitive
// They return a color and scatter another ray
class Material  {
    public:
        virtual bool Scatter(
            const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered
        ) const = 0;

        virtual Color Emitted() const {
            return Color(0,0,0);
        }
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
        DielectricMaterial(Float refractive_index) : _ref_idx(refractive_index), _albedo(Color(1.0,1.0,1.0)) {}
        DielectricMaterial(Color albedo, Float refractive_index) : _ref_idx(refractive_index), _albedo(albedo) {}

        virtual bool Scatter(
            const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered
        ) const;

    private:
        Color _albedo;
        Float _ref_idx{1};
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
        Float _fuzz{0};
};

// Emissive Material
class EmissiveMaterial : public Material {
    public:
        EmissiveMaterial(const Color& albedo) : _albedo(albedo) {}

        virtual bool Scatter(
            const Ray& r_in, const Intersection& rec, Color& attenuation, Ray& scattered
        ) const {
            return false;
        }

        virtual Color Emitted() const {
            return _albedo;
        }
    private:
        Color _albedo;

};