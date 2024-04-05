#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<vec3> verts_;
	std::vector<std::vector<int> > faces_;
	std::vector<vec2> textures_;

public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	int ntextures();
	vec3 vert(int i);
	vec2 texture(int i);
	std::vector<int> face(int idx);
};

#endif //__MODEL_H__