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

inline vec3 color(const ray &r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r,0.001,FLT_MAX,rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r,rec,attenuation,scattered)) {
            return attenuation*color(scattered, world, depth+1);
        } else {
            return vec3(0,0,0);
        }
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5,0.7,1.0);
    }
}

hitable *random_scene(int n) {
    hitable **list = new hitable*[n+1];
    list[0] = new sphere(vec3(0,-1000,0),1000, std::shared_ptr<material>(new lambertian(vec3(0.5,0.5,0.5))));
    int i=1;

    for (int a=-11; a<11; a++) {
        for (int b=-11; b<11; b++) {
            float choose_mat = randVal();
            vec3 center(a+0.9*randVal(), 0.2, b+0.9*randVal());
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.7) {
                    list[i++] = new sphere(center,0.2, std::shared_ptr<material>(
                        new lambertian(vec3(randVal() * randVal(),
                                            randVal() * randVal(),
                                            randVal() * randVal()))));
                } else if (choose_mat < 0.9) {
                    list[i++] = new sphere(center,0.2, std::shared_ptr<material>(
                        new metal(vec3(0.5*(1+randVal()),
                                        0.5*(1+randVal()),
                                        0.5*(1+randVal())),
                                        0.5*randVal())));
                } else {
                    list[i++] = new sphere(center, 0.2, std::shared_ptr<material>(
                            new dielectric(1.5)));
                }
            }
            
        }
    }
    list[i++] = new sphere(vec3(0,1,0), 1.0, std::shared_ptr<material>(new dielectric(1.5)));
    list[i++] = new sphere(vec3(-4,1,0), 1.0, std::shared_ptr<material>(new lambertian(vec3(0.4,0.2,0.1))));
    list[i++] = new sphere(vec3(4,1,0), 1.0, std::shared_ptr<material>(new metal(vec3(0.7,0.6,0.5),0.0)));
    return new hitable_list(list,i);
}

int main() {
    int width = 200; int height = 100;
    // int width = 1920; int height = 1080;
    int ns = 100; // Num samples
    std::string filename = "image.ppn";
    std::ofstream ofs;
    // binary flag is necessary on Windows
    ofs.open(filename, std::ios_base::out | std::ios_base::binary);
    ofs << "P3\n" << width << " " << height << "\n255\n";

    // const int numObjects = 5;
    // hitable *list[numObjects];
    // list[0] = new sphere(vec3(0,0,-1),0.5, std::shared_ptr<material>(new lambertian(vec3(0.8,0.3,0.3))));
    // list[1] = new sphere(vec3(0,-100.5,-1),100, std::shared_ptr<material>(new lambertian(vec3(0.8,0.8,0))));
    // list[2] = new sphere(vec3(1,0,-1),0.5, std::shared_ptr<material>(new metal(vec3(0.8,0.6,0.2))));
    // list[3] = new sphere(vec3(-1,0,-1),0.5, std::shared_ptr<material>(new dielectric(1.5)));
    // list[4] = new sphere(vec3(-1,0,-1), -0.45, std::shared_ptr<material>(new dielectric(1.5)));
    // hitable *world = new hitable_list(list,numObjects);

    hitable *world = random_scene(500);
    // camera cam(vec3(-2,2,1), vec3(0,0,-1), vec3(0,1,0), 90, float(width)/float(height));
    // camera cam(vec3(6,1,1), vec3(-4,1,0), vec3(0,1,0), 90, float(width)/float(height));
    camera cam(vec3(13,2,3),vec3(0,0,0),vec3(0,1,0),20,float(width)/float(height));
    #pragma omp parallel for ordered schedule(dynamic) { //?
    for (int j = height-1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {
            vec3 col(0,0,0);
            for (int s=0; s<ns; s++) {
                float u = float(i + randVal()) / float(width);
                float v = float(j + randVal()) / float(height);
                ray r = cam.get_ray(u,v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r,world,0);
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