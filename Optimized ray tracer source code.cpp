#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <memory>

struct Vec3 {
    double x, y, z;
    Vec3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& b) const { return Vec3(x + b.x, y + b.y, z + b.z); }
    Vec3 operator-(const Vec3& b) const { return Vec3(x - b.x, y - b.y, z - b.z); }
    Vec3 operator*(double b) const { return Vec3(x * b, y * b, z * b); }
    Vec3 mult(const Vec3& b) const { return Vec3(x * b.x, y * b.y, z * b.z); }
    Vec3& norm() { return *this = *this * (1/sqrt(x*x + y*y + z*z)); }
    double dot(const Vec3& b) const { return x*b.x + y*b.y + z*b.z; }
};

struct Ray {
    Vec3 o, d;
    Ray(const Vec3& o, const Vec3& d) : o(o), d(d) {}
};
struct Sphere {
    Vec3 center;
    double radius;

    Sphere(const Vec3& center, double radius) : center(center), radius(radius) {}

    bool intersect(const Ray& ray, double& t) const {
        Vec3 oc = ray.o - center;
        double b = oc.dot(ray.d);
        double c = oc.dot(oc) - radius * radius;
        double disc = b * b - c;
        if (disc > 0) {
            double distSqrt = sqrt(disc);
            double q = (b < 0) ? (-b - distSqrt) : (-b + distSqrt);
            t = q;
            if (t < 0) {
                t = (-b + distSqrt);
                if (t < 0) return false;
            }
            return true;
        }
        return false;
    }
};
Vec3 trace(const Ray& ray, const std::vector<Sphere>& spheres) {
    double t = 1e9;
    const Sphere* sphere = nullptr;
    for (const auto& s : spheres) {
        double d = 1e9;
        if (s.intersect(ray, d) && d < t) {
            t = d;
            sphere = &s;
        }
    }

    if (sphere) {
        Vec3 hitPoint = ray.o + ray.d * t;
        Vec3 normal = (hitPoint - sphere->center).norm();
        double diffuse = std::max(normal.dot(Vec3(1, 1, 1).norm()), 0.0);
        return Vec3(1, 1, 1) * diffuse;  // Assuming the sphere is white and lit by a white light
    }

    return Vec3(0.1, 0.1, 0.1);  // Background color
}

void render(const std::vector<Sphere>& spheres) {
    const int width = 800, height = 600;
    std::vector<Vec3> framebuffer(width * height);
    Ray camera(Vec3(0, 0, 0), Vec3(0, 0, -1)); // Simple camera

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            double x = (i - width / 2.0) / width;
            double y = (j - height / 2.0) / height;
            Ray ray(camera.o, Vec3(x, y, -1).norm());
            framebuffer[i + j * width] = trace(ray, spheres);
        }
    }

    // Output to PPM file
    std::cout << "P3\n" << width << " " << height << "\n255\n";
    for (const auto& pixel : framebuffer) {
        std::cout << static_cast<int>(std::min(1.0, pixel.x) * 255) << ' '
                  << static_cast<int>(std::min(1.0, pixel.y) * 255) << ' '
                  << static_cast<int>(std::min(1.0, pixel.z) * 255) << '\n';
    }
}
bool BVHAccel::intersect(Ray& ray) const {
  bool hit = false;

  BVHNode* to_intersect[BVHSTACKSIZE];
  int head = 0;
  to_intersect[head++] = root;

  while (head != 0) {
    assert(head < BVHSTACKSIZE);
    BVHNode* cur = to_intersect[--head];

    if (cur->bb.intersect(ray)) { // Does not modify the ray
      if (cur->isLeaf()) {
        for (const auto& primitive : cur->primitives) {
          hit |= primitive->intersect(ray); // Modifies the ray!
        }
      } else {
        to_intersect[head++] = cur->r;
        to_intersect[head++] = cur->l;
      }
    }
  }

  return hit;
}

bool BBox::intersect(const Ray& r) const {
  double txmin = (min.x - r.o.x) * r.inv_d.x;
  double txmax = (max.x - r.o.x) * r.inv_d.x;
  double tymin = (min.y - r.o.y) * r.inv_d.y;
  double tymax = (max.y - r.o.y) * r.inv_d.y;
  double tzmin = (min.z - r.o.z) * r.inv_d.z;
  double tzmax = (max.z - r.o.z) * r.inv_d.z;

  ascending(txmin, txmax);
  ascending(tymin, tymax);
  ascending(tzmin, tzmax);

  double t0 = std::max(txmin, std::max(tymin, tzmin));
  double t1 = std::min(txmax, std::min(tymax, tzmax));

  if (t1 < t0 || t0 > r.max_t || t1 < r.min_t) {
    return false;
  }

  return true;
}

void ascending(double& a, double& b) {
  if (a > b) {
    std::swap(a, b);
  }
}
int main() {
    std::vector<Sphere> spheres = {
        Sphere(Vec3(0, 0, -5), 1),
        Sphere(Vec3(2, 0, -5), 1),
        Sphere(Vec3(-2, 0, -5), 1),
    };

    render(spheres);
    return 0;
}
