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
        }else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            vec2 uv;
            for (int i=0;i<2;i++) iss >> uv[i];
            uv_.push_back(uv);
        }
    }
    // Charger toute les texture 
   // std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
    load_texture(filename, "_diffuse.tga", diffusemap_);
    load_texture(filename, "_nm.tga",      normalmap_);
    load_texture(filename, "_spec.tga",    specularmap_);
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

std::vector<vec3> Model::face(int idx) {
    return faces_[idx];
}

TGAColor Model::diffuse(vec2 uv) {
    return diffusemap_.get(uv[0], uv[1]);
}

vec3 Model::vert(int i) {
    return verts_[i];
}

vec3 Model::vert(int iface, int nthvert) {
    return verts_[faces_[iface][nthvert][0]];
}

vec2 Model::uv(int iface, int nthvert) {
    int idx = faces_[iface][nthvert][1];
    return vec2{uv_[idx][0]*diffusemap_.get_width(), uv_[idx][1]*diffusemap_.get_height()}; // Formule pour avoir la couleur qui correspond. 
}

void Model::load_texture(std::string filename, const char *suffix, TGAImage &img) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot!=std::string::npos) {
        texfile = texfile.substr(0,dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}

