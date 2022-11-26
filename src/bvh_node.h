#ifndef BVHNODE_H
#define BVHNODE_H
#include "hitable_list.h"
#include "hitable.h"
#include "aabb.h"
#include "utilities.h"
#include <algorithm>

// class bvh_node : public hitable {
// public:
//     std::shared_ptr<hitable> left;
//     std::shared_ptr<hitable> right;
//     aabb box;

//     bvh_node() {}
//     bvh_node(const hitable_list& list, float t0, float t1)
//         : bvh_node(list.objects, 0, list.objects.size(), t0, t1) {}
//     bvh_node(const std::vector<std::shared_ptr<hitable>>& src_objects,
//         size_t start, size_t end, float t0, float t1);
//     virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
//     virtual bool bounding_box(float t0, float t1, aabb& box) const;
// };

// inline bool box_compare(const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b, int axis) {
//     aabb box_a;
//     aabb box_b;

//     if (!a->bounding_box(0,0,box_a) || !b->bounding_box(0,0,box_b)) {
//         std::cerr << "No bounding box in bvh_node constructor.\n";
//     }

//     return box_a.min().e[axis] < box_b.min().e[axis];
// }

// bool box_x_compare(const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b) {
//     return box_compare(a,b,0);
// }

// bool box_y_compare(const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b) {
//     return box_compare(a,b,1);
// }

// bool box_z_compare(const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b) {
//     return box_compare(a,b,2);
// }

// bvh_node::bvh_node(const std::vector<std::shared_ptr<hitable>>& src_objects,
//     size_t start, size_t end, float t0, float t1)
// {
//     auto objects = src_objects;
//     int axis = int(randVal()*3);
//     auto comparator = (axis == 0) ? box_x_compare
//                     : (axis == 1) ? box_y_compare
//                                   : box_z_compare;

//     size_t object_span = end - start;

//     if (object_span == 1) {
//         left = right = objects[start];
//     } else if (object_span == 2) {
//         if (comparator(objects[start], objects[start+1])) {
//             left = objects[start];
//             right = objects[start+1];
//         } else {
//             left = objects[start+1];
//             right = objects[start];
//         }
//     } else {
//         std::sort(objects.begin() + start, objects.begin() + end, comparator);

//         auto mid = start + object_span/2;
//         left = std::shared_ptr<bvh_node>(new bvh_node(objects,start,mid,t0,t1));
//         right = std::shared_ptr<bvh_node>(new bvh_node(objects,mid,end,t0,t1));
//     }

//     aabb box_left, box_right;

//     if (!left->bounding_box(t0,t1,box_left)
//         || !right->bounding_box(t0,t1,box_right)) {
//             std::cerr << "No bounding box in bvh_node constructor.\n";
//         }
//     box = surrounding_box(box_left, box_right);
// }


// bool bvh_node::bounding_box(float t0, float t1, aabb& b) const {
//     b = box;
//     return true;
// }

// bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
//     // if (!box.hit(r, t_min, t_max)) return false;

//     // bool hit_left = left->hit(r,t_min,t_max,rec);
//     // bool hit_right = right->hit(r,t_min, hit_left ? rec.t : t_max, rec);

//     // return hit_left || hit_right;

//     if (box.hit(r, t_min, t_max)) {
//         hit_record left_rec, right_rec;
//         bool hit_left = left->hit(r,t_min,t_max,left_rec);
//         bool hit_right = right->hit(r,t_min,t_max,right_rec);
//         if (hit_left && hit_right) {
//             if (left_rec.t < right_rec.t) {
//                 rec = left_rec;
//             } else {
//                 rec = right_rec;
//             }
//             return true;
//         } else if (hit_left) {
//             rec = left_rec;
//             return true;
//         } else if (hit_right) {
//             rec = right_rec;
//             return true;
//         } else {
//             return false;
//         }
//     } else {
//         return false;
//     }
// }

class bvh_node : public hitable  {
    public:
        std::shared_ptr<hitable> left;
        std::shared_ptr<hitable> right;
        aabb box;

        bvh_node() {}

        bvh_node(const hitable_list& list, double time0, double time1)
            : bvh_node(list.objects, 0, list.objects.size(), time0, time1)
        {}

        bvh_node(
            const std::vector<std::shared_ptr<hitable>>& src_objects,
            size_t start, size_t end, float time0, float time1);

        virtual bool hit(
            const ray& r, float t_min, float t_max, hit_record& rec) const;

        virtual bool bounding_box(float time0, float time1, aabb& output_box) const;
};


inline bool box_compare(const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b, int axis) {
    aabb box_a;
    aabb box_b;

    if (!a->bounding_box(0,0, box_a) || !b->bounding_box(0,0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.min().e[axis] < box_b.min().e[axis];
}


bool box_x_compare (const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b) {
    return box_compare(a, b, 0);
}

bool box_y_compare (const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b) {
    return box_compare(a, b, 1);
}

bool box_z_compare (const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b) {
    return box_compare(a, b, 2);
}


bvh_node::bvh_node(
    const std::vector<std::shared_ptr<hitable>>& src_objects,
    size_t start, size_t end, float time0, float time1
) {
    auto objects = src_objects; // Create a modifiable array of the source scene objects

    int axis = randVal()*3;
    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1) {
        left = right = objects[start];
    } else if (object_span == 2) {
        if (comparator(objects[start], objects[start+1])) {
            left = objects[start];
            right = objects[start+1];
        } else {
            left = objects[start+1];
            right = objects[start];
        }
    } else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        auto mid = start + object_span/2;
        left = std::make_shared<bvh_node>(objects, start, mid, time0, time1);
        right = std::make_shared<bvh_node>(objects, mid, end, time0, time1);
    }

    aabb box_left, box_right;

    if (  !left->bounding_box (time0, time1, box_left)
       || !right->bounding_box(time0, time1, box_right)
    )
        std::cerr << "No bounding box in bvh_node constructor.\n";

    box = surrounding_box(box_left, box_right);
}


bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    if (!box.hit(r, t_min, t_max))
        return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
}


bool bvh_node::bounding_box(float time0, float time1, aabb& output_box) const {
    output_box = box;
    return true;
}

#endif