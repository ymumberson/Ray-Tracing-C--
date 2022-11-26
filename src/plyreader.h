#ifndef PLYREADER_H
#define PLYREADER_H
#include <stdio.h>
#include <fstream>
#include <string>
#include "hitable_list.h"
#include "triangle.h"
#include "bvh_node.h"
#include <sstream>
#include <vector>

// Some string helpers
// Credit: https://github.com/nmwsharp/happly/blob/master/happly.h
namespace happy {

inline std::string trimSpaces(const std::string& input) {
  size_t start = 0;
  while (start < input.size() && input[start] == ' ') start++;
  size_t end = input.size();
  while (end > start && (input[end - 1] == ' ' || input[end - 1] == '\n' || input[end - 1] == '\r')) end--;
  return input.substr(start, end - start);
}

inline std::vector<std::string> tokenSplit(const std::string& input) {
  std::vector<std::string> result;
  size_t curr = 0;
  size_t found = 0;
  while ((found = input.find_first_of(' ', curr)) != std::string::npos) {
    std::string token = input.substr(curr, found - curr);
    token = trimSpaces(token);
    if (token.size() > 0) {
      result.push_back(token);
    }
    curr = found + 1;
  }
  std::string token = input.substr(curr);
  token = trimSpaces(token);
  if (token.size() > 0) {
    result.push_back(token);
  }

  return result;
}

inline bool startsWith(const std::string& input, const std::string& query) {
  return input.compare(0, query.length(), query) == 0;
}
}; // namespace

class plyreader {
    // std::ifstream f; 
public:
    // ~plyreader() {
    //     if (f.is_open()) {
    //         f.close();
    //     }
    // }

    hitable_list readfile(const std::string filename) const {
        hitable_list world;
        std::ifstream f;
        try {
            // f.open(filename, std::ios_base::in);
            f.open(filename, std::ios_base::in | std::ios_base::binary);
            if (!f.is_open()) {
                std::cerr << "Error! Failed to open file." << std::endl;
            } 
            std::string currentline;

            for (int i=0; i<4; i++) { std::getline(f,currentline); } // Skip first 3 lines
            std::vector<std::string> tokens = happy::tokenSplit(currentline);
            std::cout << "num vertex: " << tokens[2] << std::endl;
            int numVertex = std::stoi(tokens[2]);

            for (int i=0; i<6; i++) { std::getline(f,currentline); } // Skip next 3 lines
            tokens = happy::tokenSplit(currentline);
            std::cout << "num triangles: " << tokens[2] << std::endl;
            int numTriangles = std::stoi(tokens[2]);

            for (int i=0; i<3; i++) { std::getline(f,currentline); } // Skip next 2 lines
            std::vector<vec3> pointls;
            for (int i=0; i<numVertex; i++) {
                tokens = happy::tokenSplit(currentline);
                pointls.push_back(vec3(
                    std::stof(tokens[0]),
                    std::stof(tokens[1]),
                    std::stof(tokens[2])
                ));
            }

            std::shared_ptr<material> mat(new lambertian(
                std::shared_ptr<texture>(new constant_texture(
                    vec3(0.2,0.3,0.5)
                ))));
            int v0,v1,v2;
            for (int i=0; i<numVertex; i++) {
                tokens = happy::tokenSplit(currentline);
                v0 = std::stoi(tokens[0]);
                v1 = std::stoi(tokens[1]);
                v2 = std::stoi(tokens[2]);
                world.add(std::shared_ptr<triangle>(
                    new triangle(pointls[v0],pointls[v1],pointls[v2],mat)));
            }

        } catch (...) {
            f.close();
        }
        f.close();
        return hitable_list(std::shared_ptr<bvh_node>(new bvh_node(world,0.0,1.0))); 
    }

    hitable_list readfile(std::istream& f) const {
        hitable_list world;
            std::string currentline;
            for (int i=0; i<4; i++) { std::getline(f,currentline); } // Skip first 3 lines
            std::vector<std::string> tokens = happy::tokenSplit(currentline);
            std::cout << "num vertex: " << tokens[2] << std::endl;
            int numVertex = std::stoi(tokens[2]);

            for (int i=0; i<6; i++) { std::getline(f,currentline); } // Skip next 3 lines
            tokens = happy::tokenSplit(currentline);
            std::cout << "num triangles: " << tokens[2] << std::endl;
            int numTriangles = std::stoi(tokens[2]);

            for (int i=0; i<2; i++) { std::getline(f,currentline); } // Skip next 2 lines
            std::vector<vec3> pointls;
            int scalar = 100;
            float avgx=0, avgy=0, avgz=0;
            for (int i=0; i<numVertex; i++) {
                std::getline(f,currentline);
                tokens = happy::tokenSplit(currentline);
                pointls.push_back(vec3(
                    std::stof(tokens[0])*scalar + 2.5,
                    std::stof(tokens[1])*scalar - 9.2,
                    std::stof(tokens[2])*scalar
                ));
                avgx +=std::stof(tokens[0])*scalar;
                avgy +=std::stof(tokens[1])*scalar;
                avgz += std::stof(tokens[2])*scalar;
            }

            std::cout << "avg x: " << (avgx/numVertex) << std::endl;
            std::cout << "avg y: " << (avgy/numVertex) << std::endl;
            std::cout << "avg z: " << (avgz/numVertex) << std::endl;

            std::shared_ptr<material> mat(new lambertian(
                std::shared_ptr<texture>(new constant_texture(
                    vec3(0.2,0.3,0.5)
                ))));
            int v0,v1,v2;
            for (int i=0; i<numVertex; i++) {
                std::getline(f,currentline);
                tokens = happy::tokenSplit(currentline);
                v0 = std::stoi(tokens[0]);
                v1 = std::stoi(tokens[1]);
                v2 = std::stoi(tokens[2]);
                world.add(std::shared_ptr<triangle>(
                    new triangle(pointls[v0],pointls[v1],pointls[v2],mat)));
                // std::cout << "Triangle added with coords: " << pointls[v0] << pointls[v1] << pointls[v2] << std::endl;
            }
        return hitable_list(std::shared_ptr<bvh_node>(new bvh_node(world,0.0,1.0))); 
        // return world;
    }

};

#endif