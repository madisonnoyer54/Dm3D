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

float limite = -0;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = std::abs(x0 - x1) < std::abs(y0 - y1);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = std::abs(y1 - y0);
    int error = dx / 2;
    int yStep = (y0 < y1) ? 1 : -1;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error -= dy;
        if (error < 0) {
            y += yStep;
            error += dx;
        }
    }
}

 
vec3 barycentric(vec3 pts[3], vec3 P) { 
    vec3 a = {pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0]};
    vec3 b = {pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]};
    vec3 u = cross(a, b);
    
    if (std::abs(u.z) < 1) 
        return vec3{-1, 1, 1}; // Triangle dégénéré 
    else 
        return vec3{1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z}; 
}


void triangle( vec3 p1, vec3 p2, vec3 p3, TGAImage &image, TGAColor color,  float *zbuffer){
    double w = image.get_width();
    double h = image.get_height();
    vec3 pts[3] = {p1,p2,p3};
    // On definie les variable de limite.
    vec2 x_min = vec2{( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max())};
    vec2 x_max = vec2{(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max())};

    vec2 x = {w,  h};

    // Calcule la limite de la boite englobante par rapport au 3 point
    for ( int i = 0;i<3; i ++){
        x_min.x = std::max(0.0, std::min(x_min.x, pts[i].x));
	    x_min.y = std::max(0.0, std::min(x_min.y, pts[i].y));

	    x_max.x = std::min(x.x, std::max(x_max.x, pts[i].x));
	    x_max.y = std::min(x.y, std::max(x_max.y, pts[i].y));
    }

    // Remplir les triangle 
    vec3 p;
    for (p.x = x_min.x; p.x <= x_max.x; p.x++) {
        for (p.y = x_min.y; p.y <= x_max.y; p.y++) {
            // Calculer les coordonnées barycentriques du point p par rapport au triangle
    
            vec3 bc_screen = barycentric(pts, p);

            // Vérifier si le point p est à l'intérieur du triangle en comparant les coordonnées barycentriques
            if ((bc_screen.x >= limite && bc_screen.y >= limite && bc_screen.z >= limite)||(bc_screen.x <= limite && bc_screen.y <= limite && bc_screen.z <= limite)){
                p.z = 0;
                for(int i=0; i<3; i++){
                    p.z = p.z + pts[i].z * bc_screen[i];
                }

                if( zbuffer[int(p.x + p.y * width)] <= p.z){
                    zbuffer[int(p.x + p.y * width)] = p.z;
                    image.set(p.x, p.y, color);
                }
            }
          
            
        }
    }

}

int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB); // On crée l'image

    // Dans f le 2 eme composant des 3 est l'index de la texture 
     // On recuperer l'image 
     

    // Créer une instance de la classe Model en passant le nom du fichier OBJ
    Model model("obj//african_head/african_head.obj");
    
    // Vérifier si le chargement du modèle a réussi
    if (model.nverts() == 0 || model.nfaces() == 0  || model.nuv() == 0) {
        std::cerr << "Le model n'a pas charge" << std::endl;
        return 1;
    }

     // Crée le Z buffer 
    float *zbuffer = new float[width*height];
    for (int i=0; i<width*height; i++) {
        zbuffer[i] = std::numeric_limits<int>::min();
    }

    // Pour faire les triangle 
    vec3 ligne_directrice= {0,0,-1}; // ligne directrice 
 
    TGAColor color;


    for (int i = 0; i < model.nfaces(); ++i) {
        std::vector<int> face = model.face(i); 
        vec3 coordonnee[3];
        
     

        vec3 vv[3]; 

        for (int j = 0; j < 3; j++) {
            vec3 v0 = model.vert(face[j]);
            double x = (v0.x + 1.) * width / 2.;
            double y = (v0.y + 1.) * height / 2.;

            x = std::round(x); 
            y = std::round(y); 
            coordonnee[j] = vec3{x, y, ((v0.z+1.) * 100. /2.)}; 
            
            vv[j] = v0;

            int iface = i; // L'indice de la face actuelle
            int nthvert = j; // L'indice du vertex actuel dans la face
            //vec2 uv = model.uv(iface, nthvert);
            //std::cout << "get" << uv << std::endl;
            //color = model.diffuse(uv);
          
        }

        vec3 n = cross((vv[2] - vv[0]), (vv[1] - vv[0])); 
        n = n.normalized(); // Normalisation à l'intérieur de la boucle.

        float intensite = (n * ligne_directrice) ;
        if (intensite > 0) {
            triangle(coordonnee[0], coordonnee[1], coordonnee[2],image, TGAColor(intensite * 255, intensite * 255, intensite *  255, 255), zbuffer);
        }
   
    }

   
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");


    // Utile ? 
    { 
        TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
        for (int i=0; i<width; i++) {
            for (int j=0; j<height; j++) {
                zbimage.set(i, j, TGAColor(zbuffer[i+j*width], 1));
            }
        }
        zbimage.flip_vertically(); 
        zbimage.write_tga_file("zbuffer.tga");
    }
  
    delete [] zbuffer;
 
 
    return 0;
}
