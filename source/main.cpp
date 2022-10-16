
#include <fstream>
#include "utils/utils.h"

#include "objects/hittable.h"
#include "objects/sphere.h"
//include yaml
#include "yaml-cpp/yaml.h"
#include "materials/material.h"
#include "ray_tracer/RayTracer.h"
#include "objects/aarect.h"
#include "objects/box.h"



hittable_list loadSceneFromFile(const std::string& filename) {
    YAML::Node config = YAML::LoadFile(filename + ".yaml");

    std::map <std::string, std::shared_ptr<material>> materials;
    hittable_list objects;

    for (auto material : config["materials"]) {
        auto name = material["name"].as<std::string>();
        auto type = material["type"].as<std::string>();

        if(type == "lambertian") {
            auto color_v = material["color"].as<std::vector<float>>();
            materials[name] = std::make_shared<lambertian>(color(color_v[0], color_v[1], color_v[2]));
        } else if(type == "metal") {
            auto color_v = material["color"].as<std::vector<float>>();
            auto fuzz =  random_double(0, 0.5); //material["fuzz"].as<double>();
            materials[name] = std::make_shared<metal>(color(color_v[0], color_v[1], color_v[2]), fuzz);
        } else if(type == "dielectric") {
            auto ior = material["ior"].as<float>();
            materials[name] = std::make_shared<dielectric>(ior);
        } else if(type == "diffuse_light") {
            auto color_v = material["color"].as<std::vector<float>>();
            materials[name] = std::make_shared<diffuse_light>(color(color_v[0], color_v[1], color_v[2]));
        } else {
            throw std::runtime_error("Unknown material type");
        }
    }

    for (auto object : config["objects"]) {
        auto type = object["type"].as<std::string>();
        auto material_name = object["material"].as<std::string>();
        auto material = materials[material_name];

        if(type == "sphere") {
            auto center_v = object["center"].as<std::vector<float>>();
            auto radius = object["radius"].as<float>();
            objects.add(std::make_shared<sphere>(point3(center_v[0], center_v[1], center_v[2]), radius, material));
        } else if(type == "box") {
            auto size = object["size"].as<std::vector<float>>();
            objects.add(std::make_shared<box>(point3(0, 0, 0), point3(size[0], size[1], size[2]), material));
        } else if(type == "xy_rect") {
            auto size = object["size"].as<std::vector<float>>();
            objects.add(std::make_shared<xy_rect>(size[0], size[1], size[2], size[3], size[4], material));
        } else if(type == "xz_rect") {
            auto size = object["size"].as<std::vector<float>>();
            objects.add(std::make_shared<xz_rect>(size[0], size[1], size[2], size[3], size[4], material));
        } else if(type == "yz_rect") {
            auto size = object["size"].as<std::vector<float>>();
            objects.add(std::make_shared<yz_rect>(size[0], size[1], size[2], size[3], size[4], material));
        } else {
            throw std::runtime_error("Unknown object type");
        }
    }

    return objects;
}


int main() {

    //get yaml node
    YAML::Node input = YAML::LoadFile("input.yaml");
    YAML::Node config = YAML::LoadFile("config.yaml");

    float aspect_ratio = config["ratio"][0].as<float>() / config["ratio"][1].as<float>();
    int image_width =  config["image_width"].as<int>();
    int samples_per_pixel = config["samples_per_pixel"].as<int>();
    int max_depth = config["max_depth"].as<int>();


    RayTracer tracer(image_width, aspect_ratio,max_depth,samples_per_pixel);

    // Camera and viewport
    auto viewport_height = config["camera"]["viewport_height"].as<float>();
    auto origin = point3(
            config["camera"]["origin"][0].as<float>(),
            config["camera"]["origin"][1].as<float>(),
            config["camera"]["origin"][2].as<float>()
    );
    auto look_at = point3(
            config["camera"]["look_at"][0].as<float>(),
            config["camera"]["look_at"][1].as<float>(),
            config["camera"]["look_at"][2].as<float>()
    );

    tracer.calculate_camera_and_viewport(
            aspect_ratio * viewport_height,
            viewport_height,
            config["camera"]["focal_length"].as<float>(),
            origin,
            look_at);

    // World

    auto fileName = config["scene"].as<std::string>();

    std::cout << "Loading scene from " << fileName << ".yaml" << std::endl;

    auto world = loadSceneFromFile(fileName);

    std::cout << "Rendering..." << std::endl;
    // Render
    tracer.render(world);
}