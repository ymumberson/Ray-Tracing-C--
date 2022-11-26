#ifndef HITABLE_H
#define HITABLE_H
#include "vec3.h"
#include "ray.h"
#include "utilities.h"
#include <memory>

class material;

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    std::shared_ptr<material> mat_ptr;
};

class hitable {
public:
    virtual bool hit(
        const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

class material {
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec,
        vec3& attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
public:
    vec3 albedo;

    lambertian(const vec3& a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec,
        vec3& attenuation, ray& scattered) const {
            vec3 target = rec.p + rec.normal + random_in_unit_sphere();
            scattered = ray(rec.p, target-rec.p);
            attenuation = albedo;
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