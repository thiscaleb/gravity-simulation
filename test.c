#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include <math/vector/vector3.h>

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct Body {
    //int N;
    vec4 color;
    vector3 pos;
    float radius;
    int mass;
} Body;

typedef struct objData {
    vec4 color[2];
    vector3 pos[2];
    float radius[2];
    //int mass[2];
} objData;

typedef struct Camera {
    vector3 pos;
    vector3 direction;
    GLuint ubo;
} Camera;

// struct Ray {

//   vec3 direction;
//   vec3 origin;
//   float r, theta, phi;
//   float dr, dtheta, dphi;

// };

// void debug_ray(vector3 cameraPos, vector3 dir){

//   Ray ray;
//   ray.origin = cameraPos;
//   ray.direction = dir;

//   // Convert my Ray to polar coords
//   ray.r = length(cameraPos);
//   ray.theta = acos(ray.origin.z / ray.r);
//   ray.phi = atan(ray.origin.y, ray.origin.x);

//   float dx = dir.x; // 0
//   float dy = dir.y; // 0
//   float dz = dir.z; // -1
//   ray.dr     = sin(ray.theta)*cos(ray.phi)*dx + sin(ray.theta)*sin(ray.phi)*dy + cos(ray.theta)*dz;
//   ray.dtheta = (cos(ray.theta)*cos(ray.phi)*dx + cos(ray.theta)*sin(ray.phi)*dy - sin(ray.theta)*dz) / ray.r;
//   ray.dphi   = (-sin(ray.phi)*dx + cos(ray.phi)*dy) / (ray.r * sin(ray.theta));

//   printf("POLAR COORDS = ( r: %f, theta = %f, phi = %f)\n", ray.r, ray.theta, ray.phi);
//   printf("DR, DY, DPHI = %f, %f, %f\n", ray.dr, ray.dtheta, ray.dphi);
  
// }

GLFWwindow* init_render(){

     if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    GLFWwindow* window = glfwCreateWindow(1024, 1024, "OpenGL Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);        
    
    
    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }

    // Print renderer to ensure I'm using my GPU
    printf( "Renderer: %s.\n", glGetString( GL_RENDERER ) );
    printf( "OpenGL version supported %s.\n", glGetString( GL_VERSION ) );

    return window;

}

// parse the shader file locating in shaders/*
char* parse_shader_file(const char* fpath){

    FILE *fh = fopen(fpath, "rb");

    if (fh == NULL) {
        printf("vertex file not found");
        return NULL;
    }

    int fsize;

    fseek( fh, 0, SEEK_END );
    fsize = ftell( fh );
    rewind( fh );

    char* vertex_shader = malloc(fsize + 1);

    fread(vertex_shader, sizeof( char ), fsize, fh);

    vertex_shader[fsize] = '\0';

    fclose(fh);

    return vertex_shader;

}

// lots of hardcoded values here
// it scales a range of -100000 to 100000 into -5 to 5
double scale(double val) {
    double r = -5 + ((val + 100000) * (5 + 5) / 200000);
    return r;
}

void upload_camera_ubo(Camera cam){

    struct CameraUBO{
        vec4 cameraPos;
        vec4 cameraDirection;
        vec4 cameraRight;
        vec4 cameraUp;
        vec4 tanHalfFov;
    } cameraUBOdata;

    vector3 up = {0.0f, 1.0f, 0.0f};

    // vector3 cameraFront = vec3_unit_vector(cam.direction);

    //These are the steps to calculte the vectors needed for a lookAt matrix
    // vector3 cameraTarget = add_vec3s(cameraFront, cam.pos);

    // these are the three that matter
    vector3 cameraDirection = vec3_unit_vector(cam.direction);
    vector3 cameraRight = vec3_unit_vector(cross_product(cameraDirection, up));
    vector3 cameraUp = cross_product(cameraDirection, cameraRight);

    vec4 tanHalfFov = { tanf((60.0f * 0.5f) * M_PI / 180.0f), 0, 0, 0 };

    cameraUBOdata.cameraPos = (vec4){cam.pos.x, cam.pos.y, cam.pos.z, 0};
    // printf("camera pos = %f, %f %f\n", cam.pos.x, cam.pos.y, cam.pos.z);
    cameraUBOdata.cameraDirection = (vec4){cameraDirection.x, cameraDirection.y, cameraDirection.z, 0};
    cameraUBOdata.cameraRight = (vec4){cameraRight.x, cameraRight.y, cameraRight.z, 0};
    cameraUBOdata.cameraUp = (vec4){cameraUp.x, cameraUp.y, cameraUp.z, 0};
    cameraUBOdata.tanHalfFov = tanHalfFov; // why not just put this in comp shader?

    glBindBuffer(GL_UNIFORM_BUFFER, cam.ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(cameraUBOdata), &cameraUBOdata);

}

int main(){
    GLFWwindow* window = init_render();
    glClearColor(0.0f, 0.2f, 0.2f, 1.0f);

    GLuint vao, vbo;
    GLint success;


    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,  // top left
        -1.0f, -1.0f,  0.0f, 0.0f,  // bottom left
        1.0f, -1.0f,  1.0f, 0.0f,  // bottom right

        -1.0f,  1.0f,  0.0f, 1.0f,  // top left
        1.0f, -1.0f,  1.0f, 0.0f,  // bottom right
        1.0f,  1.0f,  1.0f, 1.0f   // top right

    };

    glGenBuffers( 1, &vbo );
    glGenVertexArrays( 1, &vao );

    glBindVertexArray( vao );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);


    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    const char* f = "test.frag";
    char* fs = parse_shader_file(f);

    const char* v = "test.vert";
    char* vs = parse_shader_file(v);

    GLuint test_vs = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( test_vs, 1, (const GLchar**)&vs, NULL );
    glCompileShader( test_vs );

    GLuint test_fs = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( test_fs, 1, (const GLchar**)&fs, NULL );
    glCompileShader( test_fs );

    GLuint test_shader = glCreateProgram();
    glAttachShader( test_shader, test_vs );
    glAttachShader( test_shader, test_fs );
    glLinkProgram( test_shader );

    glGetShaderiv(test_vs, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    { 
        printf("Failed to compile vertex shader.\n");
        return -1; 
    }

    glGetShaderiv(test_fs, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    { 
        printf("Failed to compile frag shader.\n");
        return -1; 
    }

    float angle_pitch, angle_yaw;
    angle_yaw = 0.0f; 
    angle_pitch = 0.0f;
    float cameraSpeedMultiplier = 1.0f;

    float cameraSpeed = 0.001f;
    float cameraRotSpeed = 0.1f;

    // dimensions of the image
    int tex_w = 512, tex_h = 512;
    GLuint tex_output;
    glGenTextures(1, &tex_output);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_output);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT,
    NULL);
    glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    int work_grp_cnt[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

    printf("max global (total) work group counts x:%i y:%i z:%i\n",
    work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

    int work_grp_size[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
    
    GLuint work_grp_inv;
    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
    work_grp_size[0], work_grp_size[1], work_grp_size[2]);

    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    printf("max local work group invocations %i\n", work_grp_inv);

    char* ray_shader_string = parse_shader_file("test.comp");
    GLuint ray_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(ray_shader, 1, (const GLchar**)&ray_shader_string, NULL);
    glCompileShader(ray_shader);
    // check for compilation errors as per normal here

    glGetShaderiv(ray_shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    { 
        printf("Failed to compile ray shader.\n");
        return -1; 
    }

    GLuint ray_program = glCreateProgram();
    glAttachShader(ray_program, ray_shader);
    glLinkProgram(ray_program);

    unsigned int uniformBlock = glGetUniformBlockIndex(ray_program, "Bodies");
    glUniformBlockBinding(ray_program,    uniformBlock, 1);

    unsigned int uboBodies;
    glGenBuffers(1, &uboBodies);


    unsigned int cameraBlock = glGetUniformBlockIndex(ray_program, "Camera");
    glUniformBlockBinding(ray_program,    cameraBlock, 2);

    GLuint cameraUBO = 0;
    glGenBuffers(2, &cameraUBO);

    srand(time(NULL));

    int NUM_OBJECTS = 15;

    // These must be set to 16 to match the memory mapping in my comp shader
    // If these are changed remember to CHANGE THE ONES IN THE COMP SHADER

    vec4 bodiesColor[16];
    vec4 bodiesPos[16];
    // Even though this should just be a float, std 140 padding is weird and requires a 16byte stride for scalars
    vec4 bodiesRadii[16];

    // randomly get color
    for(int i=0; i < NUM_OBJECTS; i++){

        // 0.00 => 1.00
        float r = (rand() % 10) * 0.1;
        float g = (rand() % 10) * 0.1;
        float b = (rand() % 10) * 0.1;

        vec4 color = {r, g, b, 1.0};

        bodiesColor[i] = color;
    }


    // randomly get position
    for(int i=0; i < NUM_OBJECTS; i++){

        // -10.00 => 10.00
        float x = ( 10 - (rand() % 200) * 0.1);
        float y = ( 10 - (rand() % 200) * 0.1);
        float z = (rand() % 100) * -0.1; // keep z infront of camera ( it must be negative )

        vec4 pos = {x, y, z, 0.0};

        bodiesPos[i] = pos;
    }

    bodiesPos[0] = (vec4){0,0,5,0};

    bodiesPos[1] = (vec4){3.0,0.0,-3.0,0};

    bodiesPos[2] = (vec4){5.0,3.0,0.0,0};


    // randomly get radius
    for(int i=0; i < NUM_OBJECTS; i++){

        // float r = (rand() % 100) * 0.05;

        //DEBUG - remove later
        float r = 1.0f;

        vec4 radius = {r, 0, 0, 0};

        bodiesRadii[i] = radius;
    }


    // int bodiesMass[16] = {1000, 1000};

    size_t bodiesColorSize = sizeof(bodiesColor);
    size_t bodiesPosSize = sizeof(bodiesPos);
    size_t bodiesRadiiSize = sizeof(bodiesRadii);

    size_t TOTAL_SIZE = bodiesColorSize + bodiesPosSize + bodiesRadiiSize;

    printf("Total Size of UBO = %d", TOTAL_SIZE);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferData(GL_UNIFORM_BUFFER, TOTAL_SIZE, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);
    
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboBodies, 0, TOTAL_SIZE);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, bodiesColorSize, &bodiesColor);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferSubData(GL_UNIFORM_BUFFER, bodiesColorSize, bodiesPosSize, &bodiesPos);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferSubData(GL_UNIFORM_BUFFER, bodiesColorSize + bodiesPosSize, bodiesRadiiSize, &bodiesRadii);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);

    Camera cam;
    cam.pos = (vector3){0.1f, 0.1f, 10.0f};

    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, 80, NULL, GL_DYNAMIC_DRAW); 
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, cameraUBO); // binding = 2 matches shader
    cam.ubo = cameraUBO;

    const double G = (6.6742E-11);
    // const double G = (1);

    double m1 = 100;
    double m2 = 100000;

    double r = 10; // hardcode distance between 

    double vx1 = 0;
    double vx2 = 0;

    double vy1 = 5;
    double vy2 = 0;

    double vz1 = 0;
    double vz2 = 0;

    // double x1 = pos.x;
    // double x2 = pos2.x;

    // double y1 = pos.y;
    // double y2 = pos2.y;

    double x1 = 100000;
    double x2 = 0;

    double y1 = 0;
    double y2 = 0;

    double z1 = 0;
    double z2 = 0;

    float delta_t = 10.0f;


    double lastTime = glfwGetTime(); // Time of the last debug message
    float lastFrame = 0.0f; // Time of the last frame
    float deltaTime = 0.0f;	// Time between current frame and last frame
    int nbFrames = 0;
    int run = 0;

    while( !glfwWindowShouldClose(window) ){


        float yaw = angle_yaw * 3.14159f / 180.0f;
        float pitch = angle_pitch * 3.14159f / 180.0f;

        // This is used to handle rotation of the camera
        vector3 direction;
        direction.x = sin(yaw) * cos(pitch);
        direction.y = sin(pitch);
        direction.z = -cos(yaw) * cos(pitch);  
        cam.direction = vec3_unit_vector(direction);

        // Super basic FPS count
        nbFrames++;
        run++;
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime; 

        if ( currentTime - lastTime >= 1.0){ // If last prinf() was more than 1 sec ago

            printf("\nCurrent Frame = %d", run);
            printf("\n%f ms/frame", 1000.0/(double)(nbFrames));
            printf("\nFPS: %f", (double)nbFrames / 1.0);
            printf("\nRendering With: %s", glGetString(GL_RENDERER));
            printf("\n CAMERA POS = %f, %f, %f", cam.pos.x, cam.pos.y, cam.pos.z);

            lastTime += 1.0;
            nbFrames = 0;
        }

        double ax1 = G * m2 * ((x2 - x1) / (r * r * r));
        double ax2 = G * m1 * ((x1 - x2) / (r * r * r));

        double ay1 = G * m2 * ((y2 - y1) / (r * r * r));
        double ay2 = G * m1 * ((y1 - y2) / (r * r * r));

        double az1 = G * m2 * ((z2 - z1) / (r * r * r));
        double az2 = G * m1 * ((z1 - z2) / (r * r * r));

        // printf("Accel of b1 and b2 = (%lf, %lf), (%lf, %lf)\n", ax1, ay1, ax2, ay2);

        vx1 = vx1 + delta_t * ax1;
        vx2 = vx2 + delta_t * ax2;

        vy1 = vy1 + delta_t * ay1;
        vy2 = vy2 + delta_t * ay2;

        vz1 = vz1 + delta_t * az1;
        vz2 = vz2 + delta_t * az2;

        // printf("Vel of b1 and b2 = (%lf, %lf), (%lf, %lff)\n", vx1, vy1, vx2, vy2);

        x1 = x1 + delta_t * vx1;
        x2 = x2 + delta_t * vx2;

        y1 = y1 + delta_t * vy1;
        y2 = y2 + delta_t * vy2;

        z1 = z1 + delta_t * vz1;
        z2 = z2 + delta_t * vz2;


        // printf("Pos of b1 and b2 = (%lf, %lf, %lf) (%lf,%lf, %lf)\n", scale(x1), scale(y1), scale(z1), scale(x2), scale(y2), scale(z2));

        bodiesPos[0].x = scale(x1);
        bodiesPos[0].y = scale(y1);
        bodiesPos[1].x = scale(x2);
        bodiesPos[1].y = scale(y2);
        bodiesPos[0].z = scale(z1);
        bodiesPos[1].z = scale(z2);


        { // launch compute shaders!
            glUseProgram(ray_program);
            glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);
        }

            //make sure writing to image has finished before read
            glMemoryBarrier(
                GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
                GL_TEXTURE_FETCH_BARRIER_BIT
            );

            glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);

            upload_camera_ubo(cam);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
                cam.pos.z -= 0.01f;
            }


            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
                cam.pos.z += 0.01f;
                // printf("Camera pos z = %f", cam.pos.z);
            }

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
                cam.pos.x -= 0.01f;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
                cam.pos.x += 0.01f;

            }

            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
                cam.pos.y -= 0.1f;
            }

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
                cam.pos.y += 0.1f;
            }


            // x rotation
            if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
                angle_pitch -= cameraRotSpeed;
                
            }
            if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
                angle_pitch += cameraRotSpeed; 
            }

            // y rotation
            if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
                angle_yaw -= cameraRotSpeed; 
            }
            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
                angle_yaw += cameraRotSpeed; 
            }

            float identity[16] = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };


            glUseProgram(ray_program);
            GLuint posLoc = glGetUniformLocation(ray_program, "cam");
            glUniform3f(posLoc, cam.pos.x, cam.pos.y, cam.pos.z);
            // printf("cam x,y,z = %f, %f, %f\n", cam.x, cam.y, cam.z);

            glUseProgram(ray_program);
            GLuint pitchLoc = glGetUniformLocation(ray_program, "pitch");
            glUniform1f(pitchLoc, angle_pitch);

            glUseProgram(ray_program);
            GLuint pitchYaw = glGetUniformLocation(ray_program, "yaw");
            glUniform1f(pitchLoc, yaw);

            GLuint frameCounter = glGetUniformLocation(ray_program, "frame");
            glUniform1i(frameCounter, run);

            // printf("yaw = %f\n", cos(yaw));


            // glUseProgram(test_shader);
            // GLuint modelLoc = glGetUniformLocation(test_shader, "model");
            // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, identity);

            // glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
            // glBufferSubData(GL_UNIFORM_BUFFER, bodiesColorSize, bodiesPosSize, &bodiesPos);
            // glBindBuffer(GL_UNIFORM_BUFFER, 1);


        { // normal drawing pass
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(test_shader);
            glBindVertexArray(vao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex_output);
            glDisable(GL_DEPTH_TEST);  // draw as background
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glEnable(GL_DEPTH_TEST);
        }
            // Wipe the drawing surface clear.
            // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glfwSwapBuffers( window );
            glfwPollEvents();

    }
    glfwDestroyWindow(window);
    glfwTerminate();

}