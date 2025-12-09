#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include "utils/shaders_parser.h"
#include "math/vector/vector3.h"
#include <assert.h>


typedef struct sphere{
    vector3 pos;
    float radius;
    int num_segments;
    GLuint vbo;
    GLuint vao;
} sphere;

typedef struct camera{
    vector3 pos;
} camera;

// Function to draw a circle at (cx, cy) with radius 
void drawTestCircle(vector3 c, float r, int num_segments) {

    // Center of circle
    float cx = c.x;
    float cy = c.y;
    float cz = c.z;

    int num_vertices = num_segments * 3;

    int idx = 0;

    float *vertices = malloc((num_vertices+1) * sizeof(float));
    
    for (int i = 0; i <= num_segments; i++) {
        float angle = 2.0f * 3.141592f * i / num_segments;
        float x = cx + cosf(angle) * r;
        float y = cy + sinf(angle) * r;

        vertices[idx] = x;
        vertices[idx+1] = y;
        vertices[idx+2] = cz;

        idx += 3;

    }

    GLuint vbo = 0;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, (num_segments+1) * 3* sizeof( float ), vertices, GL_STATIC_DRAW );

    GLuint vao = 0;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
    glEnableVertexAttribArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

    glDrawArrays(GL_TRIANGLE_FAN, 0, num_segments + 1);


}

int drawTestSphere(sphere *s) {

    // Center of sphere
    float cx = s->pos.x;
    float cy = s->pos.y;
    float cz = s->pos.z;

    int idx = 0;

    int n = s->num_segments;
    float r = s->radius;

    vector3 *vertices = malloc(sizeof(vector3) * (13 * (s->num_segments * s->num_segments)));
    
    for (float i = 0.0f; i <= n ; i++) {

        float theta = 3.141592f * (i / n);
        float theta1 = 3.141592f * ((i+1) / n);

        for(float j = 0.0f; j <= n; j++){

            float phi = 2.0f * 3.1415926535f * (j / n);
            float phi1 = 2.0f * 3.141592f * ((j+1) / n);


            float x1 = cx + (sinf(theta) * cosf(phi) * r);
            float y1 = cy + (sinf(theta) * sinf(phi) * r);
            float z1 = cz + (cosf(theta) * r);

            float x2 = cx + (sinf(theta1) * cosf(phi) * r);
            float y2 = cy + (sinf(theta1) * sinf(phi) * r);
            float z2 = cz + (cosf(theta1) * r);

            float x3 = cx + (sinf(theta) * cosf(phi1) * r);
            float y3 = cy + (sinf(theta) * sinf(phi1) * r);
            float z3 = cz + (cosf(theta) * r);

            float x4 = cx + (sinf(theta1) * cosf(phi1) * r);
            float y4 = cy + (sinf(theta1) * sinf(phi1) * r);
            float z4 = cz + (cosf(theta1) * r);


            vector3 p1 = {x1, y1, z1};
            vector3 p2 = {x2, y2, z2};
            vector3 p3 = {x3, y3, z3};
            vector3 p4 = {x4, y4, z4};


            //normals for the mesh
            vector3 p12 = subtract_vec3s(p2, p1);
            vector3 p13 = subtract_vec3s(p3, p1);

            vector3 n1 = cross_product(p12, p13);

            // triangle 1
            vertices[idx++] = p1;
            vertices[idx++] = n1;

            vertices[idx++] = p2;
            vertices[idx++] = n1;

            vertices[idx++] = p3;
            vertices[idx++] = n1;

            // triangle 2
            vertices[idx++] = p3;
            vertices[idx++] = n1;

            vertices[idx++] = p4;
            vertices[idx++] = n1;

            vertices[idx++] = p2;
            vertices[idx++] = n1;

        }

    }

    glBindBuffer( GL_ARRAY_BUFFER, s->vbo );
    glBufferData(GL_ARRAY_BUFFER, idx * sizeof(vector3), vertices, GL_STATIC_DRAW);

    glBindVertexArray( s->vao );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_DOUBLE, GL_FALSE, 2* sizeof(vector3), (void*)0 );

    glEnableVertexAttribArray(1);
    // this has an offset - it tracks the normals of each mesh
    glVertexAttribPointer( 1, 3, GL_DOUBLE, GL_FALSE, 2* sizeof(vector3), (void*)sizeof(vector3) );

    // glDrawArrays(GL_TRIANGLES, 0, idx/2);

    // free(vertices);

    return vertices;

}


int main() {

     if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }


    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    GLFWwindow* window = glfwCreateWindow(1600, 900, "OpenGL Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);        
    
    
    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

      // Try to call some OpenGL functions, and print some more version info.
  printf( "Renderer: %s.\n", glGetString( GL_RENDERER ) );
  printf( "OpenGL version supported %s.\n", glGetString( GL_VERSION ) );

//   float points[] = {
//    0.5f,  0.5f,  0.0f, // x,y,z of first point.
//    0.5f, -0.5f,  0.0f, // x,y,z of second point.
//   -0.5f, -0.5f,  0.0f,
//   -0.5f,  0.5f,  0.0f,  
// };





vector3 center = {0.0f,0.0f, 9.0f};
vector3 center2 = {0.2f, 0.4f, -5.0f};

sphere *sphere1 = malloc(sizeof(sphere));
sphere *sphere2 = malloc(sizeof(sphere));

sphere1->pos = center;
sphere2->pos = center2;

sphere1->num_segments = 60;
sphere2->num_segments = 60;

sphere1->radius = 0.3f;
sphere2->radius = 0.2f;

glGenBuffers( 1, &sphere1->vbo );
glGenVertexArrays( 1, &sphere1->vao );
glGenBuffers( 1, &sphere2->vbo );
glGenVertexArrays( 1, &sphere2->vao );


// yes I know it's hardcoded.... I don't care
const char* fpath = "shaders/test.vert";

char* vertex_shader = parse_shader_file(fpath);

const char* frag_path = "shaders/test.frag";
char* fragment_shader = parse_shader_file(frag_path);

GLuint vs = glCreateShader( GL_VERTEX_SHADER );
glShaderSource( vs, 1, (const GLchar**)&vertex_shader, NULL );
glCompileShader( vs );
GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
glShaderSource( fs, 1, (const GLchar**)&fragment_shader, NULL );
glCompileShader( fs );

GLuint shader_program = glCreateProgram();
glAttachShader( shader_program, fs );
glAttachShader( shader_program, vs );
glLinkProgram( shader_program );


glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

double prev_s = glfwGetTime();  // Set the initial 'previous time'.
double title_countdown_s = 0.1;

float angle_deg = 0.0f;

camera *cam = malloc(sizeof(camera));

vector3 camera_pos = {0,0,-10};
cam->pos = camera_pos;

GLuint objectColorLoc = glGetUniformLocation(shader_program, "objectColor");
glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
int idx = drawTestSphere(sphere1);
glUniform3f(objectColorLoc, 0.3f, 0.3f, 0.3f);
drawTestSphere(sphere2);

while ( !glfwWindowShouldClose( window ) ) {
    double curr_s = glfwGetTime(); // Get the current time. 
	double elapsed_s  = curr_s - prev_s; // Work out the time elapsed over the last frame.
	prev_s            = curr_s;          // Set the 'previous time' for the next frame to use.

	// Print the FPS, but not every frame, so it doesn't flicker too much.
	title_countdown_s -= elapsed_s;

	if ( title_countdown_s <= 0.0 && elapsed_s > 0.0 ) {
		double fps        = 1.0 / elapsed_s;

		// Create a string and put the FPS as the window title.
		char tmp[256];
		sprintf( tmp, "FPS %.2lf", fps );
		glfwSetWindowTitle(window, tmp );
		title_countdown_s = 0.1;
	}

    // Update window events.
    glfwPollEvents();
    
    // Wipe the drawing surface clear.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Put the shader program, and the VAO, in focus in OpenGL's state machine.
    glUseProgram( shader_program );

    float angle = angle_deg * 3.14159f / 180.0f;
    float c = cos(angle);
    float s = sin(angle);

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    const float cameraSpeed = 0.0005f; 


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        cam->pos.z += cameraSpeed; 
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cam->pos.z -= cameraSpeed; 
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        cam->pos.x -= cameraSpeed; 
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        cam->pos.x += cameraSpeed; 
    }


    float x = cam->pos.x;
    float y = cam->pos.y;
    float z = cam->pos.z;

    float view[16] = {
        1,  0, 0, 0,
        0,  1,  0, 0,
        0,  0,  1, 0,
        -x, -y, z, 1   
    };

    //increment angle deg
    angle_deg = angle_deg + 0.5f;

    float fov = 60.0f * 3.14159f / 180.0f;
    float aspect = 1600.0f / 900.0f;
    float near = 0.1f;
    float far = 1000.0f;

    float f = 1.0f / tanf(fov / 2.0f);

    float projection[16] = {
        f/aspect, 0, 0,  0,
        0, f, 0,  0,
        0, 0, (far+near)/(near-far), -1,
        0, 0, (2*far*near)/(near-far), 0
    };

    GLuint projLoc = glGetUniformLocation(shader_program, "projection");
    glUseProgram(shader_program);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

    GLuint viewLoc = glGetUniformLocation(shader_program, "view");
    glUseProgram(shader_program);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

    float model[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glUseProgram( shader_program );
    GLuint modelLoc = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat *)model);

    // set lightpos
    GLuint lightPos = glGetUniformLocation(shader_program, "lightPos");
    glUniform3f(lightPos, 0.0f, 0.0f, -5.0f);


    float lightModel[16] = {
        c,  0, -s, 0,
        0,  1,  0, 0,
        s,  0,  c, 0,
        0,  0, -10, 1 
    };

    GLuint lightModelLoc = glGetUniformLocation(shader_program, "lightModel");
    glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, (const GLfloat *)lightModel);

    //set color of frag
    GLuint lightColorLoc = glGetUniformLocation(shader_program, "lightColor");
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);


    glBindVertexArray( sphere1->vao );
    glDrawArrays(GL_TRIANGLES, 0, idx/2);


    // Put the stuff we've been drawing onto the visible area.
    glfwSwapBuffers( window );
    }


}