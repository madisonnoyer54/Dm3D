#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<vec3> verts_;
	std::vector<std::vector<vec3> > faces_;
	std::vector<vec2> uv_;
	std::vector<vec3> norms_;
	TGAImage diffusemap_;
    TGAImage normalmap_;
    TGAImage specularmap_;

public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	int nuv();
	vec3 vert(int i);
	vec2 uv(int i);
	vec3 normal(vec2 uv);
	void load_texture(std::string filename, const char *suffix, TGAImage &img);
	std::vector<int> face(int idx);
	std::vector<int>  faceTexture(int idx);
	vec3 normal(int iface, int nthvert);
	float specular(vec2 uv);
	TGAColor diffuse(vec2 uv);
	vec2 uv(int iface, int nthvert);
	vec3 vert(int iface, int nthvert);

};

#endif //__MODEL_H__