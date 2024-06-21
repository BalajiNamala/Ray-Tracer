//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"
#define TINYOBJLOADER_USE_DOUBLE

// Use robust triangulation by using Mapbox earcut.
#define TINYOBJLOADER_USE_MAPBOX_EARCUT

#define TINYOBJLOADER_IMPLEMENTATION

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "tiny_obj_loader.h"
struct Triangle {
	
	std::array<int,3> vertices;
	std::array<int,3> normals;
	std::array<int,3> tex;
	int material;
	Triangle(std::array<int,3> v, std::array<int,3> n, std::array<int,3> t, int m) : vertices(v), normals(n), tex(t), material(m) {};
};
void load_triangles(const tinyobj::shape_t & shape, std::vector<Triangle> & triangles) {
    //convert a tinyobjloader shape_t object containing indices into vertex properties and textures
    //into a vector of Triangle objects grouping these indices
    const std::vector<tinyobj::index_t> & indices = shape.mesh.indices;
    const std::vector<int> & mat_ids = shape.mesh.material_ids;

    std::cout << "Loading " << mat_ids.size() << " triangles..." << std::endl;

    for(size_t face_ind = 0; face_ind < mat_ids.size(); face_ind++) {
        triangles.push_back(
            Triangle(
                {indices[3*face_ind].vertex_index, indices[3*face_ind+1].vertex_index, indices[3*face_ind+2].vertex_index},
                {indices[3*face_ind].normal_index, indices[3*face_ind+1].normal_index, indices[3*face_ind+2].normal_index},
                {indices[3*face_ind].texcoord_index, indices[3*face_ind+1].texcoord_index, indices[3*face_ind+2].texcoord_index},
                mat_ids[face_ind]
                ));
    }
}

int main() {
    
std::string inputfile = "TriangleObj.obj";
tinyobj::attrib_t attrib;
std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;
std::string warn;
std::string err;
//bool ret=true;
bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn,&err, inputfile.c_str(),NULL,true,true);

if (!warn.empty()) {
    std::cout << "Warning: " << warn << std::endl;
}

if (!err.empty()) {
    std::cerr << "Error: " << err << std::endl;
}

if (!ret) {
    exit(1);
}
std::vector<Triangle> triangles;

for(auto shape = shapes.begin(); shape < shapes.end(); ++shape) {
    load_triangles(*shape, triangles);
}
    hittable_list world;



    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

   /*/ for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }*/

   /*auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    /*auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));*/

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 10;
    cam.max_depth         = 20;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
}
