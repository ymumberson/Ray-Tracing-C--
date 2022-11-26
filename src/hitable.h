#ifndef HITABLE_H
#define HITABLE_H
#include "vec3.h"
#include "ray.h"
#include "aabb.h"
#include "utilities.h"
#include <memory>
#include <iostream>
#include "rtw_stb_image.h"

class material;

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    std::shared_ptr<material> mat_ptr;
    float u;
    float v;
};

class hitable {
public:
    virtual bool hit(
        const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};

class texture {
public:
    virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture {
public:
    vec3 color;
    constant_texture() {}
    constant_texture(vec3 c) : color(c) {}
    virtual vec3 value(float u, float v, const vec3& p) const {
        return color;
    }
};

class checker_texture : public texture {
public:
    std::shared_ptr<texture> odd;
    std::shared_ptr<texture> even;
    checker_texture() {}
    checker_texture(std::shared_ptr<texture> t0, std::shared_ptr<texture> t1)
        : odd(t0), even(t1) {}
    virtual vec3 value(float u, float v, const vec3& p) const {
        float sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
        if (sines < 0) {
            return odd->value(u,v,p);
        } else {
            return even->value(u,v,p);
        }
    }
};

class image_texture : public texture {
    public:
        const static int bytes_per_pixel = 3;

        image_texture()
          : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

        image_texture(const char* filename) {
            auto components_per_pixel = bytes_per_pixel;

            data = stbi_load(
                filename, &width, &height, &components_per_pixel, components_per_pixel);

            if (!data) {
                std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
                width = height = 0;
            }

            bytes_per_scanline = bytes_per_pixel * width;
        }

        ~image_texture() {
            STBI_FREE(data);
        }

        virtual vec3 value(float u, float v, const vec3& p) const {
            // If we have no texture data, then return solid cyan as a debugging aid.
            if (data == nullptr)
                return vec3(0,1,1);

            // Clamp input texture coordinates to [0,1] x [1,0]
            if (u > 1) u = 1;
            if (u < 0) u = 0;
            if (v > 1) v = 1;
            if (v < 0) v = 0;
            v = 1.0 - v;

            auto i = static_cast<int>(u * width);
            auto j = static_cast<int>(v * height);

            // Clamp integer mapping, since actual coordinates should be less than 1.0
            if (i >= width)  i = width-1;
            if (j >= height) j = height-1;

            const auto color_scale = 1.0 / 255.0;
            auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;

            return vec3(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
        }

    private:
        unsigned char *data;
        int width, height;
        int bytes_per_scanline;
};

class material {
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec,
        vec3& attenuation, ray& scattered) const = 0;
    virtual vec3 emitted(float u, float v, const vec3& p) const {
        return vec3(0,0,0);
    }
};

class diffuse_light : public material {
public:
    std::shared_ptr<texture> emit;
    diffuse_light(std::shared_ptr<texture> a) : emit(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec,
        vec3& attenuation, ray& scattered) const {
            return false;
    }
    virtual vec3 emitted(float u, float v, const vec3& p) const {
        return emit->value(u,v,p);
    }
};

class lambertian : public material {
public:
    std::shared_ptr<texture> albedo;

    lambertian(std::shared_ptr<texture> a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec,
        vec3& attenuation, ray& scattered) const {
            vec3 target = rec.p + rec.normal + random_in_unit_sphere();
            scattered = ray(rec.p, target-rec.p);
            attenuation = albedo->value(rec.u,rec.v,rec.p);
            return true;
    }
};

class metal : public material {
public:
    vec3 albedo;
    float fuzz;
    metal(const vec3& a) : albedo(a), fuzz(1) {}
    metal(const vec3& a, float f) : albedo(a) {
        if (f < 1) {
            fuzz = f;
        } else {
            fuzz = 1;
        }
    }
    virtual bool scatter(const ray& r_in, const hit_record& rec,
        vec3& attenuation, ray& scattered) const {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p,reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            return (dot(scattered.direction(),rec.normal) > 0);
    }
};

class dielectric : public material {
public:
    float ref_idx;
    dielectric(float ri) : ref_idx(ri) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec,
        vec3& attenuation, ray& scattered) const {
            vec3 outward_normal;
            vec3 reflected = reflect(r_in.direction(), rec.normal);
            float ni_over_nt;
            attenuation = vec3(1,1,1);
            vec3 refracted;
            float reflect_prob;
            float cosine;
            if (dot(r_in.direction(), rec.normal) > 0) {
                outward_normal = -rec.normal;
                ni_over_nt = ref_idx;
                cosine = ref_idx * dot(r_in.direction(), rec.normal) /
                            r_in.direction().length();
            } else {
                outward_normal = rec.normal;
                ni_over_nt = 1.0 / ref_idx;
                cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
            }
            if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
                reflect_prob = schlick(cosine, ref_idx);
            } else {
                // scattered = ray(rec.p, reflected); // Not needed
                reflect_prob = 1.0;
            }
            if (randVal() < reflect_prob) {
                scattered = ray(rec.p, reflected);
            } else {
                scattered = ray(rec.p, refracted);
            }
            return true;
    }
};

#endif