#ifndef HITABLELIST_H
#define HITABLELIST_H
#include "hitable.h"
#include "aabb.h"
#include <memory>
#include <vector>

class hitable_list: public hitable {
public:
    std::vector<std::shared_ptr<hitable>> objects;

    hitable_list() {}
    hitable_list(std::shared_ptr<hitable> object) { add(object); }
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
    void add(std::shared_ptr<hitable> object) {objects.push_back(object); }
};

bool hitable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

bool hitable_list::bounding_box(float t0, float t1, aabb& box) const {
    if (objects.empty()) return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto& object : objects) {
        if (!object->bounding_box(t0, t1, temp_box)) return false;
        box = first_box ? temp_box : surrounding_box(box, temp_box);
        first_box = false;
    }

    
    return true;
}

#endif