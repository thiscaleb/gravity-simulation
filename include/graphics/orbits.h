#ifndef ORBITS_H
#define ORBITS_H

#include "utils/utils.h"
#include "utils/structures.h"

void initOrbit(points_list* orbit);
void drawOrbit(const points_list* orbits_list);
void updateOrbits(points_list* orbits_list, vector3 coords);
GLuint init_orbit_shaders();

#endif
