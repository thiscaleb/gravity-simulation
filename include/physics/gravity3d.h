#ifndef GRAVITY3D_H
#define GRAVITY3D_H

#include "math/vector/vector3.h"
#include "utils/structures.h"

vector3 f_x_3d(double t, vector3 x, vector3 v);
vector3 f_v_nbody(double t, vector3 pos_self, body_3d* bodies[], int index, int N);
void rk4_nbody_3d(double t, double h, body_3d* bodies[], int N);

#endif