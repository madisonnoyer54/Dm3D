#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) { // Sommet
            iss >> trash;
            vec3 v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) { // Ligne des facette 
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                idx--; 
                f.push_back(idx);
            }
            faces_.push_back(f);
        }else if (! line.compare(0, 2, "vt")){
            iss >> trash;
            vec2 uv;
            for (int i=0;i<2;i++){
                iss >> uv[i];
            } 
            textures_.push_back({uv.x, 1-uv.y});
        }
    }
}

Model::~Model() {
}


int Model::nverts() {
    return (int)verts_.size();
}

/**
 * Fontion qui retourne le nombre de face 
*/
int Model::nfaces() {
    return (int)faces_.size();
}

int Model::ntextures() {
    return (int)textures_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}



vec3 Model::vert(int i) {
    return verts_[i];
}

vec2 Model::texture(int i) {
    return textures_[i];
}

