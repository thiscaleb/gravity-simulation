#ifndef RENDER_H
#define RENDER_H

#include "utils/utils.h"
#include "utils/structures.h"
#include "utils/constants.h"

double* drawCircle(vector2 c, float r, int num_segments);
void initOrbit(points_list* orbit);
void drawOrbits(points_list* orbits_list, int N);
void updateOrbits(points_list* orbits_list, vector3 coords);
int render(body_2d* bodies_array[], int ref_frame_code, float timeskip, int num_bodies, bool debug);

#endif