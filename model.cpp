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
            std::vector<vec3> f;
            vec3 tmp;
            iss >> trash;
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                for (int i=0; i<3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }else if (!line.compare(0, 2, "vt")){
            iss >> trash;
            vec2 uv;
            for (int i=0;i<2;i++){
                iss >> uv[i];
            } 
            uv_.push_back({uv.x, 1-uv.y});
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
    load_texture("obj//african_head/african_head", "_diffuse.tga", diffusemap_);
    load_texture("obj//african_head/african_head", "_nm.tga",      normalmap_);
    load_texture("obj//african_head/african_head", "_spec.tga",    specularmap_);

}

Model::~Model() {
}


int Model::nverts() {
    return (int)verts_.size();
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

/**
 * Fontion qui retourne le nombre de face 
*/
int Model::nfaces() {
    return (int)faces_.size();
}

int Model::nuv() {
    return (int)uv_.size();
}

std::vector<int> Model::face(int idx) {
    std::vector<int> face;
    for (int i=0; i<(int)faces_[idx].size(); i++) face.push_back(faces_[idx][i][0]);
    return face;
}

vec3 Model::vert(int i) {
    return verts_[i];
}

vec2 Model::uv(int i) {
    return uv_[i];
}

TGAColor Model::diffuse(vec2 uv) {
        


    return diffusemap_.get(uv[0], uv[1]);
}

// Méthode pour obtenir les coordonnées de texture (uv) d'un sommet spécifique d'un triangle dans le modèle
vec2 Model::uv(int iface, int nthvert) {

    // Obtient l'index du sommet dans le tableau de coordonnées de texture (uv_)
    int idx = faces_[iface][nthvert][1];
        std::cout << "Indice de sommet dans le tableau de coordonnées de texture : " << idx << std::endl;

    // Retourne les coordonnées de texture du sommet ajustées en fonction de la taille de la texture
    // Multiplie les coordonnées de texture brutes par la largeur et la hauteur de la texture pour les ajuster à l'échelle de l'image
    return vec2{uv_[idx][0] * diffusemap_.get_width(), uv_[idx][1] * diffusemap_.get_height()};
}