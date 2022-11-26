#include <iostream>
#include <fstream>
#include <string>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "hitable.h"
#include "hitable_list.h"
#include "camera.h"
#include <cfloat>
#include "utilities.h"
#include <memory>
#include <omp.h>
#include "bvh_node.h"
#include "triangle.h"
#include <fstream>
#include <string>
#include "plyreader.h"

inline vec3 color(const ray &r, const hitable& world, int depth) {
    hit_record rec;

    if (depth <= 0) {
        return vec3(0,0,0);
    }

    if (!world.hit(r, 0.001, FLT_MAX, rec)) { // return background colour
        // return vec3(0,0,0);
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5,0.7,1.0);
    }

    ray scattered;
    vec3 attenuation;
    vec3 emitted = rec.mat_ptr->emitted(rec.u,rec.v,rec.p);
    if (!rec.mat_ptr->scatter(r,rec,attenuation,scattered)) {
        return emitted;
    } else {
        return emitted + attenuation*color(scattered, world, depth-1);
    }
}

hitable_list random_scene() {
    hitable_list world;
    std::shared_ptr<texture> floor_texture(new checker_texture(
        std::shared_ptr<texture>(new constant_texture(vec3(0.2,0.3,0.1))),
        std::shared_ptr<texture>(new constant_texture(vec3(0.9,0.9,0.9)))
    ));
    world.add(std::shared_ptr<sphere>(new sphere(vec3(0,-1000,0),1000, std::shared_ptr<material>(new lambertian(
        floor_texture)))));
    for (int a=-11; a<11; a++) {
        for (int b=-11; b<11; b++) {
            float choose_mat = randVal();
            vec3 center(a+0.9*randVal(), 0.2, b+0.9*randVal());
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.5) {
                    world.add(std::shared_ptr<sphere>(new sphere(center,0.2, std::shared_ptr<material>(
                        new lambertian(
                            std::shared_ptr<texture>(
                                new constant_texture(vec3(randVal() * randVal(),
                                            randVal() * randVal(),
                                            randVal() * randVal()))))))));
                } else if (choose_mat < 0.7) {
                    world.add(std::shared_ptr<sphere>(new sphere(center,0.2, std::shared_ptr<material>(
                        new metal(vec3(0.5*(1+randVal()),
                                        0.5*(1+randVal()),
                                        0.5*(1+randVal())),
                                        0.5*randVal())))));
                } else if (choose_mat < 0.8) {
                    world.add(std::shared_ptr<sphere>(new sphere(center, 0.2, std::shared_ptr<material>(
                            new dielectric(1.5)))));
                } else {
                    world.add(std::shared_ptr<sphere>(new sphere(center, 0.2, std::shared_ptr<material>(
                            new diffuse_light(
                                std::shared_ptr<texture>(new constant_texture(vec3(1,1,1)))
                            )))));
                }
            }
            
        }
    }
    std::shared_ptr<texture> img(
        new image_texture("img_textures/globe.jpg")
    );


    world.add(std::shared_ptr<sphere>(new sphere(vec3(0,1,0), 1.0, std::shared_ptr<material>(new dielectric(1.5)))));
    // world.add(std::shared_ptr<sphere>(new sphere(vec3(-4,1,0), 1.0, std::shared_ptr<material>(new lambertian(std::shared_ptr<texture>(new constant_texture(vec3(vec3(0.4,0.2,0.1)))))))));
    world.add(std::shared_ptr<sphere>(new sphere(vec3(-4,1,0), 1.0, std::shared_ptr<material>(new lambertian(img)))));
    world.add(std::shared_ptr<sphere>(new sphere(vec3(4,1,0), 1.0, std::shared_ptr<material>(new metal(vec3(0.7,0.6,0.5),0.0)))));
    
    return hitable_list(std::shared_ptr<bvh_node>(new bvh_node(world,0.0,1.0)));
    // return world;
}

hitable_list triangleTest() {
    hitable_list world;
    vec3 p0 = vec3(1,1,1);
    vec3 p1 = vec3(-1,-1,-1);
    vec3 p2 = vec3(2,-2,2);

    vec3 p3 = vec3(1,1+1,1);
    vec3 p4 = vec3(-1,-1+1,-1);
    vec3 p5 = vec3(2,-2+1,2);
    world.add(std::shared_ptr<hitable>(new triangle(
        p0,p1,p2,
        std::shared_ptr<material>(new lambertian(
                std::shared_ptr<texture>(new constant_texture(
                    vec3(1.0,0.0,0.0)
                ))))
    )));
    world.add(std::shared_ptr<hitable>(new triangle(
        p3,p4,p5,
        std::shared_ptr<material>(new lambertian(
                std::shared_ptr<texture>(new constant_texture(
                    vec3(1.0,0.0,0.0)
                ))))
    )));
    return world;
}

hitable_list dragon_model() {
    // std::ifstream f("models/bun_zipper.ply");
    // return plyreader().readfile("models/bun_zipper.ply");
    // return plyreader().readfile("models/bun_zipper.ply");
    std::ifstream f;
    f.open("models/bun_zipper_res4.ply", std::ios_base::in | std::ios_base::binary);
            if (!f.is_open()) {
                std::cerr << "Error! Failed to open file." << std::endl;
            }
    hitable_list ls = plyreader().readfile(f);
    f.close();
    return ls;
}

int main() {
    // hitable_list world = random_scene();
    hitable_list world = dragon_model();
    // hitable_list world = triangleTest();

    // const int width = 200; const int height = 100;
    // const int width = 1280; const int height = 720;
    const int width = 1920; const int height = 1080;
    // const int width = 3840; const int height = 2160;
    const int ns = 1; // Num samples
    std::string filename = "image.ppn";
    std::ofstream ofs;
    // binary flag is necessary on Windows
    ofs.open(filename, std::ios_base::out | std::ios_base::binary);
    if (!ofs.is_open()) {
        std::cerr << "Error! Failed to open file." << std::endl;
    }
    ofs << "P3\n" << width << " " << height << "\n255\n";

    // camera cam(vec3(13,2,3),vec3(0,0,0),vec3(0,1,0),20,float(width)/float(height));
    camera cam(vec3(13,2,3),vec3(0,0,0),vec3(0,1,0),90,float(width)/float(height));

    std::cout << "Begining render!\n";
    #pragma omp for schedule(dynamic)
    for (int j=height-1; j>=0; --j) {
        for (int i = 0; i < width; ++i) {
            vec3 col(0,0,0);
            for (int s=0; s<ns; s++) {
                float u = float(i + randVal()) / float(width);
                float v = float(j + randVal()) / float(height);
                ray r = cam.get_ray(u,v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r,world,50);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]),sqrt(col[1]),sqrt(col[2]));
            int ir = (int(255.9*col[0]));
            int ig = (int(255.9*col[1]));
            int ib = (int(255.9*col[2]));
            ofs << ir << " " << ig << " " << ib << "\n";
        } 
    }
    ofs.close(); 
}