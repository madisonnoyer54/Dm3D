#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;


void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
    bool steep = false; 
    if (std::abs(x0-x1)<std::abs(y0-y1)) { 
        std::swap(x0, y0); 
        std::swap(x1, y1); 
        steep = true; 
    } 
    if (x0>x1) { 
        std::swap(x0, x1); 
        std::swap(y0, y1); 
    } 
    int dx = x1-x0; 
    int dy = y1-y0; 
    int derror2 = std::abs(dy)*2; 
    int error2 = 0; 
    int y = y0; 
    for (int x=x0; x<=x1; x++) { 
        if (steep) { 
            image.set(y, x, color); 
        } else { 
            image.set(x, y, color); 
        } 
        error2 += derror2; 
        if (error2 > dx) { 
            y += (y1>y0?1:-1); 
            error2 -= dx*2; 
        } 
    } 
} 
/*
void triangle(vec2 t0, vec2 t1, vec2 t2, TGAImage &image, TGAColor color) {
    if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
    if (t0.y>t1.y) std::swap(t0, t1);
    if (t0.y>t2.y) std::swap(t0, t2);
    if (t1.y>t2.y) std::swap(t1, t2);
    int total_height = t2.y-t0.y;
    for (int i=0; i<total_height; i++) {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
        vec2 A =               t0 + (t2-t0)*alpha;
        vec2 B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;
        if (A.x>B.x) std::swap(A, B);
        for (int j=A.x; j<=B.x; j++) {
            image.set(j, t0.y+i, color); // attention, due to int casts t0.y+i != A.y
        }
    }
}
*/
int main(int argc, char** argv) {
     TGAImage image(800, 800, TGAImage::RGB);
    //image.set(0, 0, red); // Color the point (0,0) with red

    // Créer une instance de la classe Model en passant le nom du fichier OBJ
    Model model("obj//african_head/african_head.obj");

    
    // Vérifier si le chargement du modèle a réussi
    if (model.nverts() == 0 || model.nfaces() == 0) {
        std::cerr << "Failed to load model." << std::endl;
        return 1;
    }


    // Afficher les points des sommets commençant par "v" ( ppour afficher un nuage )
    //std::cout << "Vertices starting with 'v':" << std::endl;
    /*
    for (int i = 0; i < model.nverts(); ++i) {
        vec3 vertex = model.vert(i);
        std::cout << "v " << vertex[0] << " " << vertex[1] << " " << vertex[2] << std::endl;
        image.set(vertex[0]*64+64,vertex[1]*64+64,white);


    }*/


    std::cout << "Faces:" << std::endl;
    for (int i = 0; i < model.nfaces(); ++i) {
     
        
        std::vector<int> face =  model.face(i); 
        for (int j=0; j<3; j++) { 
            vec3 v0 = model.vert(face[j]); 
            vec3 v1 = model.vert(face[(j+1)%3]); 
            int x0 = (v0.x+1.)*width/2.; 
            int y0 = (v0.y+1.)*height/2.; 
            int x1 = (v1.x+1.)*width/2.; 
            int y1 = (v1.y+1.)*height/2.; 
            std::cout << "x is equal to " << y0; 
            line(x0, y0, x1, y1, image, white); 
        } 
    }

  //  line(13, 20, 80, 40, image, white);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
 
    return 0;
}
