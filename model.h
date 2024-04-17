#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<vec3> verts_;
	std::vector<std::vector<vec3> > faces_;
	std::vector<vec2> uv_;
	TGAImage diffusemap_;
    TGAImage normalmap_;
    TGAImage specularmap_;
	void load_texture(std::string filename, const char *suffix, TGAImage &img);

public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	vec3 vert(int i);
	vec3 vert(int iface, int nthvert);
	std::vector<vec3> face(int idx);
	vec2 uv(int iface, int nthvert);
	TGAColor diffuse(vec2 uv);
};

#endif //__MODEL_H__