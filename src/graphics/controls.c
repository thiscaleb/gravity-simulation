#include "graphics/controls.h"
#include "graphics/render3d.h"

// All the if statements for keypresses in GLFW
// Updates the camera struct as needed
void get_input(GLFWwindow* window, camera* cam){
    
    vector3 cameraPosDefault = {0, 0.4f,1.5f};

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    // move camera forward / back
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        cam->pos = add_vec3s(cam->pos, scale_vec3(cam->front, cam->speed));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cam->pos = add_vec3s(cam->pos, scale_vec3(cam->front, (-1) * cam->speed));
    }
    // move camera left / right
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        // Splitting this into two lines to make it a bit cleaner
        vector3 temp = vec3_unit_vector(cross_product(cam->front, cam->up)); 
        cam->pos = add_vec3s(cam->pos, scale_vec3(temp, (-1) * cam->speed));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        vector3 temp = vec3_unit_vector(cross_product(cam->front, cam->up)); 
        cam->pos = add_vec3s(cam->pos, scale_vec3(temp, cam->speed));    
    }
    // move cam up
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        cam->pos = add_vec3s(cam->pos, scale_vec3(cam->up, (-1) * cam->speed));
    }
    // move cam down
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        cam->pos = add_vec3s(cam->pos, scale_vec3(cam->up, cam->speed));
    }

    // x rotation
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
        cam->pitch -= cam->rotSpeed;
        
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
        cam->pitch += cam->rotSpeed; 
    }

    // y rotation
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
        cam->yaw -= cam->rotSpeed; 
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
        cam->yaw += cam->rotSpeed; 
    }

    // this adjusts the camera speed
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){

        //Prevent the print statement from popping up hundreds of time per press
        if(cam->speedMultiplier != 1){
            cam->speedMultiplier = 1;
            puts("\nCamera speed set to 1x");
        }
    }
    // 2x speed
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){

        //Prevent the print statement from popping up hundreds of time per press
        if(cam->speedMultiplier != 2){
            cam->speedMultiplier = 2;
            puts("\nCamera speed set to 2x");
        }
    }
    // 5x speed
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS){
        //Prevent the print statement from popping up hundreds of time per press
        if(cam->speedMultiplier != 5){
            cam->speedMultiplier = 5;
            puts("\nCamera speed set to 5x");
        }
    }

    // Jump to a body (useful on larger scales when stuff is small)
    // I'll need to edit the lookAt matrix to make the camera look at the object
    // if (glfwGetKey(window, GLFW_KEY_UP)){
    //     cam->pos = normalize_vec3(bodies_array[1]->pos, SPACE_MIN, SPACE_MAX);
    //     cam->pos.x = cam->pos.x + 0.01f;
    //     cam ->pos.y = cam->pos.y + 0.01f;
    // }

    // reset camera to init
    if (glfwGetKey(window, GLFW_KEY_R)){
        cam->pos = cameraPosDefault;
        // Also need to reset the rotation
        cam->yaw = 0.0f;
        cam->yaw = -90.0f;
    }

    // Don't let the user's flip the camera over, it breaks things
    if(cam->pitch > 89.0f)
        cam->pitch =  89.0f;
    if(cam->pitch < -89.0f)
        cam->pitch = -89.0f;

        
}