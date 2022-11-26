#ifndef UTILITIES_H
#define UTILITIES_H
#include "vec3.h"
#include <cmath>

inline float randVal() {
    return float(rand()) / float(RAND_MAX);
}

inline vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0*vec3(randVal(),randVal(),randVal())
            - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}

inline void get_sphere_uv(const vec3& p, float& u, float& v) {
    // float phi = atan2(p.z(),p.x());
    // float theta = asin(p.y());
    // u = 1-(phi + M_PI) / (2*M_PI);
    // v = (theta + M_PI/2) / (M_PI);

    float phi = atan2(-p.z(),p.x()) + M_PI;
    float theta = acos(-p.y());
    u = phi / (2*M_PI);
    v = theta / M_PI;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

inline bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    } else {
        return false;
    }
}

inline float schlick(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1-cosine),5);
}

#endif