#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "vec3.h"
#include "hitable.h"
#include <memory>

class triangle : public hitable {
public:
    vec3 v0, v1, v2;
    vec3 norm;
    std::shared_ptr<material> mat_ptr;

    triangle() {}
    triangle(vec3 p0, vec3 p1, vec3 p2,  std::shared_ptr<material> m)
    : mat_ptr(m) {
        v0 = p0;
        v1 = p1;
        v2 = p2;
    };
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
    inline virtual vec3 calculateNorm() const {
        vec3 v0v1 = v1 - v0;
        vec3 v0v2 = v2 - v0;
        return cross(v0v2,v0v1);
    }
};

bool triangle::hit(
    const ray& r, float tmin, float tmax, hit_record& rec
) const {
    vec3 v0v1 = v1 - v0;
    vec3 v0v2 = v2 - v0;
    vec3 pvec = cross(r.direction(),v0v2);
    float det = dot(v0v1,pvec);

    //If triangle culling
    // if (det < FLT_EPSILON) return false;

    //If not triangle culling
    if (abs(det) < FLT_EPSILON) return false;

    float invDet = 1.0 / det;
    vec3 tvec = r.origin() - v0;
    rec.u = dot(tvec,pvec) * invDet;
    if (rec.u < 0 || rec.u > 1) return false;

    vec3 qvec = cross(tvec,v0v1);
    rec.v = dot(r.direction(),qvec) * invDet;
    if (rec.v < 0 || rec.u + rec.v > 1) return false;

    rec.t = dot(v0v2,qvec) * invDet;
    rec.p = r.point_at_parameter(rec.t);
    rec.mat_ptr = mat_ptr;
    rec.normal = calculateNorm();

    return true;
}

bool triangle::bounding_box(float t0, float t1, aabb& box) const {
    float padding = 0.001f;
    vec3 min = vec3(fmin(fmin(v0.x(),v1.x()),v2.x())-padding,
                    fmin(fmin(v0.y(),v1.y()),v2.y())-padding,
                    fmin(fmin(v0.z(),v1.z()),v2.z())-padding);
    vec3 max = vec3(fmax(fmax(v0.x(),v1.x()),v2.x())+padding,
                    fmax(fmax(v0.y(),v1.y()),v2.y())+padding,
                    fmax(fmax(v0.z(),v1.z()),v2.z())+padding);
    box = aabb(min,max);
    return true;
}

#endif