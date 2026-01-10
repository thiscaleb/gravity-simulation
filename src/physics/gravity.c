#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "math/math_funcs.h"
#include "math/vector/vector2.h"
#include "physics/gravity.h"

// find the center of gravity from a N-body system
static vector2 find_nbody_cog(body_2d* bodies[], int num_bodies){
    
    // sum up the mass of the bodies
    double total_m = 0;
    vector2 pos = {0.0, 0.0};
    for(int i = 0; i < num_bodies; i++){
        total_m += bodies[i]->mass;
        vector2 w = scale_vec2(bodies[i]->pos, bodies[i]->mass);
        pos = add_vec2s(pos, w);
    }

    pos = scale_vec2(pos, (1 / total_m));

    return pos;
}

// equation of motion in reference frame attached to b1, where mass b1 >> b2
// still using 2d equation
void relative_equation_of_motion( body_2d *b1,  body_2d *b2, float delta_t){

    double x = b2->pos.x;
    double y = b2->pos.y;
    //m is the mass of the stationary object, body1
    runge_kutta(0, delta_t, b1->mass , b2);

    double r = sqrt(((0.0 - x)*(0.0 - x)) + ((0.0 - y)*(0.0 - y))); // since this is relative frame, the other set would be (0)

    double alt = fabs(r) - b1->radius;
    if(alt < 0.0){
        printf("\nObject crashed into planet!");
        exit(0);
    }

}

void rk4_equation_of_motion( body_2d *b1, body_2d *b2, float delta_t){

    coint_runge_kutta(0, delta_t, b1, b2);

    double r = sqrt(((b1->pos.x - b2->pos.x)*(b1->pos.x - b2->pos.x)) + ((b1->pos.y - b2->pos.y)*(b1->pos.y - b2->pos.y)));

    if (r <= b1->radius + b2->radius){
        printf("\n B1 and B2 have collided. Ending simulation....");
        exit(0);
    }

}

//Find the COG, and run calcualtions relative to that
void rk4_relative_equation_of_motion( body_2d *b1, body_2d *b2, float delta_t){

    cog_ref_runge_kutta(0, delta_t, b1, b2);

    double r = sqrt(((b1->pos.x - b2->pos.x)*(b1->pos.x - b2->pos.x)) + ((b1->pos.y - b2->pos.y)*(b1->pos.y - b2->pos.y)));

    if (r <= b1->radius + b2->radius){
        printf("\n B1 and B2 have collided. Ending simulation....");
        exit(0);
    }
}

