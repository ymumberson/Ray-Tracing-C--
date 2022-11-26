#ifndef SPHERE_H
#define SPHERE_H
#include "vec3.h"
#include "hitable.h"
#include <memory>

class sphere : public hitable {
public:
    vec3 center;
    float radius;
    std::shared_ptr<material> mat_ptr;

    sphere() {}
    sphere(vec3 cen, float r,  std::shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc,oc) - radius*radius;
    float discriminant = b*b - a*c;
    // if (discriminant > 0) {
    //     float temp = (-b - sqrt(b*b-a*c))/a;
    //     if (temp < t_max && temp > t_min) {
    //         rec.t = temp;
    //         rec.p = r.point_at_parameter(rec.t);
    //         rec.normal = (rec.p - center) / radius;
    //         return true;
    //     }
    //     temp = (-b + sqrt(b*b-a*c))/a;
    //     if (temp < t_max && temp > t_min) {
            // rec.t = temp;
            // rec.p = r.point_at_parameter(rec.t);
            // rec.normal = (rec.p - center) / radius;
    //         return true;
    //     }
    // }

    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);
    auto root = (-b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.point_at_parameter(rec.t);
    rec.normal = (rec.p - center) / radius;
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif