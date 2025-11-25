#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include "utils/shaders_parser.h"
#include "math/vector/vector3.h"
#include <assert.h>

GLuint vao, vbo, ebo;


// Function to draw a circle at (cx, cy) with radius 
float* drawTestCircle(vector3 c, float r, int num_segments) {

    // Center of circle
    float cx = c.x;
    float cy = c.y;

    int num_vertices = num_segments * 3;

    int idx = 0;

    float *vertices = malloc((num_vertices+1) * sizeof(float));
    
    for (int i = 0; i <= num_segments; i++) {
        float angle = 2.0f * 3.141592f * i / num_segments;
        float x = cx + cosf(angle) * r;
        float y = cy + sinf(angle) * r;

        vertices[idx] = x;
        vertices[idx+1] = y;
        vertices[idx+2] = 0.0;

        idx += 3;

    }

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

vector3 center = {0.0f,0.0f, 0.0f};
int num_segments = 50;
float* points = drawTestCircle(center, 0.2, num_segments);


GLuint vbo = 0;
glGenBuffers( 1, &vbo );
glBindBuffer( GL_ARRAY_BUFFER, vbo );
glBufferData( GL_ARRAY_BUFFER, num_segments * 3* sizeof( float ), points, GL_STATIC_DRAW );

GLuint vao = 0;
glGenVertexArrays( 1, &vao );
glBindVertexArray( vao );
glEnableVertexAttribArray( 0 );
glBindBuffer( GL_ARRAY_BUFFER, vbo );
glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

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


glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

double prev_s = glfwGetTime();  // Set the initial 'previous time'.
double title_countdown_s = 0.1;

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


    int time_loc = glGetUniformLocation( shader_program, "time" );
    assert( time_loc > -1 ); // NB. include assert.h for assert().

    // Update window events.
    glfwPollEvents();
    
    // Wipe the drawing surface clear.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Put the shader program, and the VAO, in focus in OpenGL's state machine.
    glUseProgram( shader_program );
    glUniform1f( time_loc, (float)curr_s);
    glBindVertexArray( vao );

    // Draw points 0-3 from the currently bound VAO with current in-use shader.
    glDrawArrays(GL_TRIANGLE_FAN, 0, num_segments);
    //   glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

        // Put the stuff we've been drawing onto the visible area.
    glfwSwapBuffers( window );
    }


}