#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "math/vector/vector2.h"
#include "math/vector/vector3.h"
#include <glad.h>
#include "utils/utils.h"

typedef struct 
{
    double mass; // in kg
    double radius; //in m
    // Each body needs to keep track of its own VBO/VAO to update its rendering
    GLuint vbo;
    GLuint vao;
    vector2 pos;
    vector2 velocity;
    points_list* orbit;
} body_2d;

typedef struct 
{
    double mass; // in kg
    double radius; //in m
    GLuint vbo;
    GLuint vao;
    vector3 pos;
    vector3 velocity;
} body_3d;

#endif