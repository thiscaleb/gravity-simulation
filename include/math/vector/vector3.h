#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>

//3 Dimensional Vector
typedef struct 
{
    float x; 
    float y;
    float z;

} vector3;

//3 Dimensional Vector
typedef struct 
{
    double x; 
    double y;
    double z;

} dvector3;

// normalize a two_d_vectpr to something that opengl can render
vector3 normalize_vec3(vector3 vec, double min, double max);
vector3 denormalize_vec3(vector3 vec, double min, double max);

// Scale up a vector vec by s
vector3 scale_vec3(vector3 vec, double s);

// Add two vectors and return their sum
vector3 add_vec3s(vector3 vec1, vector3 vec2);

//Add 4 vector3s (used for RK4)
vector3 add_4vec3s(vector3 v1, vector3 v2, vector3 v3, vector3 v4);

//subtract two vec2s from eachother
vector3 subtract_vec3s(vector3 vec1, vector3 vec2);

vector3 vec3_unit_vector(vector3 v);

double vec3_distance_between(vector3 v1, vector3 v2);

double dot_vec3s(vector3 vec1, vector3 vec2);

vector3 cross_product(vector3 vec1, vector3 vec2);

vector3 dvector3_to_vector3(dvector3 vec);

#endif