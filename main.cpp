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
 
vec3 barycentric(vec2 pts[3], vec2 P) { 
    vec3 a = {pts[2][0] - pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0]};
    vec3 b = {pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]};
    vec3 u = cross(a, b);
    
    if (std::abs(u.z) < 1) 
        return vec3{-1, 1, 1}; // Triangle dégénéré 
    else 
        return vec3{1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z}; 
}



void triangle( vec2 p1, vec2 p2, vec2 p3, TGAImage &image, TGAColor color){
    vec2 pts[3] = {p1,p2,p3};
    // On definie les variable de limite.
    vec2 x_min = {image.get_width()-1,  image.get_height()-1};
    vec2 x_max = {0,0};

    vec2 x = {image.get_width()-1,  image.get_height()-1};

    // Calcule la limite de la boite englobante par rapport au 3 point
    for ( int i = 0;i<3; i ++){
        x_min.x = std::max(0.0, std::min(x_min.x, pts[i].x));
	    x_min.y = std::max(0.0, std::min(x_min.y, pts[i].y));

	    x_max.x = std::min(x.x, std::max(x_max.x, pts[i].x));
	    x_max.y = std::min(x.y, std::max(x_max.y, pts[i].y));
    }

    // Remplir les triangle 
    vec2 p;
    for (p.x=x_min.x; p.x<=x_max.x; p.x++) { 
        for (p.y=x_min.y; p.y<=x_max.y; p.y++) { 
            vec3 bc_screen  = barycentric(pts, p); 
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue; 
            image.set(p.x, p.y, color); 
        } 
    } 

}

int main(int argc, char** argv) {
     TGAImage image(width, height, TGAImage::RGB);

    // Créer une instance de la classe Model en passant le nom du fichier OBJ
    Model model("obj//african_head/african_head.obj");
    
    // Vérifier si le chargement du modèle a réussi
    if (model.nverts() == 0 || model.nfaces() == 0) {
        std::cerr << "Failed to load model." << std::endl;
        return 1;
    }


    // Afficher la face en point 
    //std::cout << "Vertices starting with 'v':" << std::endl;
    /*
    for (int i = 0; i < model.nverts(); ++i) {
        vec3 vertex = model.vert(i);
        std::cout << "v " << vertex[0] << " " << vertex[1] << " " << vertex[2] << std::endl;
        image.set(vertex[0]*64+64,vertex[1]*64+64,white);


    }*/


    // Pour faire la face en segment 
    /*
    for (int i = 0; i < model.nfaces(); ++i) {
        std::vector<int> face =  model.face(i); 
        for (int j=0; j<3; j++) { 
            vec3 v0 = model.vert(face[j]); 
            vec3 v1 = model.vert(face[(j+1)%3]); 
            int x0 = (v0.x+1.)*width/2.; 
            int y0 = (v0.y+1.)*height/2.; 
            int x1 = (v1.x+1.)*width/2.; 
            int y1 = (v1.y+1.)*height/2.; 
            line(x0, y0, x1, y1, image, white); 
        } 
    }*/

    // Pour faire les triangle 
    
    for (int i = 0; i < model.nfaces(); ++i) {
        std::vector<int> face =  model.face(i); 
        vec2 coordonnee1;
        vec2 coordonnee2;
        vec2 coordonnee3;


        
        for (int j=0; j<3; j++) {
            vec3 v0 = model.vert(face[j]);
            vec3 v1 = model.vert(face[(j+1)%3]); 
            vec3 v2 = model.vert(face[(j+2)%3]); 
        
            coordonnee1.x = (v0.x+1.)*width/2.; 
            coordonnee1.y = (v0.y+1.)*height/2.;
            coordonnee2.x = (v1.x+1.)*width/2.;
            coordonnee2.y = (v1.y+1.)*height/2.;
            coordonnee3.x = (v2.x+1.)*width/2.; 
            coordonnee3.y = (v2.y+1.)*height/2.;
            triangle(coordonnee1, coordonnee2, coordonnee3, image, TGAColor(rand()%255, rand()%255, rand()%255, 255));

        }
       
        
    }



    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
 
    return 0;
}
