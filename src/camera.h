#ifndef CAMERA_H
#define CAMERA_H
#include "ray.h"
#include <cmath>

class camera {
public:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    camera() {
        lower_left_corner = vec3(-2.0,-1.0,-1.0);
        horizontal = vec3(4.0,0.0,0.0);
        vertical = vec3(0.0,2.0,0.0);
        origin = vec3(0.0,0.0,0.0);
    }

    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) {
        vec3 u, v, w;
        float theta = vfov*M_PI/180.0;
        float half_height = tan(theta/2.0);
        float half_width = aspect * half_height;
        origin = lookfrom;
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup,w));
        v = cross(w,u);
        lower_left_corner = vec3(-half_width, -half_height, -1.0);
        lower_left_corner = origin - half_width*u - half_height*v - w;
        horizontal = 2*half_width*u;
        vertical = 2*half_height*v;
    }

    ray get_ray(float u, float v) {
        return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
    }
};

#endif