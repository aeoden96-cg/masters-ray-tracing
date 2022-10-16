cmake_minimum_required(VERSION 3.16)
project(PROJECT)
set(CMAKE_CXX_STANDARD 17)


ADD_EXECUTABLE(PROJECT
        source/main.cpp
        source/output.h
        source/output.cpp
        source/objects/hittable.cpp
        source/objects/hittable.h
        source/objects/sphere.cpp
        source/objects/sphere.h
        source/objects/hittable_list.cpp
        source/objects/hittable_list.h
        source/materials/material.cpp
        source/materials/material.h
        source/camera/camera.cpp
        source/camera/camera.h
        source/ray/ray.cpp
        source/ray/ray.h
        source/utils/utils.cpp
        source/utils/utils.h
        source/ray_tracer/RayTracer.cpp
        source/ray_tracer/RayTracer.h
        source/materials/texture.cpp
        source/materials/texture.h
        source/objects/aarect.cpp
        source/objects/aarect.h
        source/objects/bounding-boxes/aabb.cpp
        source/objects/bounding-boxes/aabb.h
        source/objects/bvh.cpp
        source/objects/bvh.h
        source/objects/box.cpp source/objects/box.h)

target_link_libraries(
        PROJECT LINK_PUBLIC -lyaml-cpp
#        -lGLEW -lGL -lGLU -lglut -lpthread -lm
        )
# FIND ALL YAML FILES
file(GLOB_RECURSE YAML_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.yaml")

# copy config.yaml and input.yaml to build directory
foreach(YAML_FILE ${YAML_FILES})
    configure_file(${YAML_FILE} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()







