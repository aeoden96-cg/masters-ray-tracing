#ifndef PROJECT_RAYTRACER_H
#define PROJECT_RAYTRACER_H

#include <fstream>
#include "../utils/utils.h"
#include "../objects/hittable.h"
#include "../objects/hittable_list.h"
#include "../camera/camera.h"
#include "../output.h"
#include "../materials/material.h"



class RayTracer {

public:
    RayTracer(int width, float aspect_ratio,int max_depth,int samples_per_pixel);

    void render(const hittable_list& world);

    void calculate_camera_and_viewport(
      float viewport_width,
      float viewport_height,
      float focal_length,
      point3 org,
      point3 look_at
      );

    /**
     * @brief Ispušta zraku i vraća boju koja dobivena na sljedeći način:
     *  1. Ako je zraka udarila u objekt, vraća se boja objekta
     *  2. Ako zraka nije udarila u objekt, vraća se boja pozadine
     * @param r zraka
     * @param background boja pozadine
     * @param world skup svih objekata u sceni
     * @param depth dubina rekurzije
     * @return konačna boja
     */
    color trace_ray(const ray& r, const color& background, const hittable& world, int depth);

private:
    int width;
    int height;
    int max_depth;
    int samples_per_pixel;
    //float aspect_ratio;
    point3 origin;
    point3 look_at;
    glm::vec3 horizontal;
    glm::vec3 vertical;
    point3 lower_left_corner;
};

#endif //PROJECT_RAYTRACER_H