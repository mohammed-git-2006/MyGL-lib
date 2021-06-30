#define __LINUX__
#include "lib/OBJLoader.hpp"
#include "lib/opengl.hpp"
#include "lib/Display.hpp"
#include "lib/Texture.hpp"
#include "lib/ShaderProgram.hpp"
#include "lib/Keyboard.hpp"
#include "lib/Mouse.hpp"
#include "lib/VertexArray.hpp"
#include "lib/VerticesLoader.hpp"
#include "lib/RenderObject.hpp"
#include "lib/Debugger.hpp"
#include "lib/Projection.hpp"

Display display;
RenderObject cube;
Projection projection;
MatrixObject view;
Position lightPosition;

float camera_speed = 10;

void MoveCamera() {
    if (Keyboard::isPressed(GLFW_KEY_W))
        view.Translate(0, 0, 1 * display.delta_time * camera_speed);
    
    if (Keyboard::isPressed(GLFW_KEY_S))
        view.Translate(0, 0, -1 * display.delta_time * camera_speed);

    if (Keyboard::isPressed(GLFW_KEY_A))
        view.Translate(1 * display.delta_time * camera_speed, 0, 0);

    if (Keyboard::isPressed(GLFW_KEY_D))
        view.Translate(-1 * display.delta_time * camera_speed, 0, 0);

    if (Keyboard::isPressed(GLFW_KEY_PAGE_UP)) 
        view.Translate(0, -1 * display.delta_time * camera_speed, 0);

    if (Keyboard::isPressed(GLFW_KEY_PAGE_DOWN)) 
        view.Translate(0, 1 * display.delta_time * camera_speed, 0);
}

int main(int c, char ** args) {
    Debugger::Log("Init", "Initializing Display ...");
    display.Make("GLFW window", 700, 700);
    display.EnableBlend();
    display.EnableDepth();

    GLFWimage* image = new GLFWimage;
    int comp;
    //image->height = 512;
    //image->width  = 512;
    image->pixels = stbi_load("res/opengl.jpg", &image->width, &image->height, &comp, STBI_rgb_alpha);

    std::cout << image->width << ", " << image->height << std::endl;

    std::cout << sizeof(unsigned char) << std::endl;

    memset(image->pixels, 0xff, 16 * 16 * 3);

    glfwSetWindowIcon(display.glfwWindow, 1, image);

    Debugger::Log("Init", "Initializing Keyboard and Mouse ...");
    Keyboard::setDisplay(&display);
    Mouse::setDisplay(&display);

    Debugger::Log("Init", "Initializing Projection ...");
    projection.setProjection_Perspictive(80, 1, 0.1, 200);

    

    Debugger::Log("Init", "Initializing Cube ...");
    cube.shader.loadFromFile("res/cube_shader.hlsl");
    cube.shader.Compile();

    if (! cube.shader.status) {
        Debugger::Error("Init", "CUBE_SHADER:\n" + cube.shader.error);
        display.Terminate();
        return -1;
    }

    cube.texture.loadFromFile("res/grass.jpg");
    cube.texture.GenerateTexture();
    cube.vl.loadFromFile("res/cube_vertices.vrt");
    cube.va.Init(8 * FLOAT_SIZE, GL_QUADS);
    cube.va.AddLayout(3);
    cube.va.AddLayout(2);
    cube.va.AddLayout(3);
    cube.va.LoadToBuffer(cube.vl.vertices, cube.vl.size * FLOAT_SIZE, GL_DYNAMIC_DRAW);

    float sky_diff = 0.05;

    glClearColor(0.53 - sky_diff, 0.81 - sky_diff, 0.92 - sky_diff, 1);
    view.Translate(-25, -2, -20);
    lightPosition.Move(1, 1, 0);

    OBJLoader* obj_loader = new OBJLoader;
    obj_loader->loadFromFile("res/stall.obj");
    std::cout << "OBJ_Loader::error :\n\t" << obj_loader->error << std::endl;

    Debugger::Log("Init", "obj loader : temp_vertices : " + std::to_string(obj_loader->temp_vertices.size()));
    Debugger::Log("Main", "Main Loop Entry");

    RenderObject stall;
    
    stall.va.Init(0, GL_QUADS);
    stall.va.AddLayout(3, &(obj_loader->vertices[0]), obj_loader->vertices.size() * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    stall.va.AddLayout(2, &(obj_loader->texCoords[0]), obj_loader->texCoords.size() * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    stall.va.AddLayout(3, &(obj_loader->normals[0]), obj_loader->normals.size() * FLOAT_SIZE, GL_DYNAMIC_DRAW);

    stall.texture = CreateTexture("res/stallTexture.png");

    while (! display.shouldClose() && !Keyboard::isPressed(GLFW_KEY_Q)) {
        display.Clear_Depth();
        display.Clear();
        display.calculateDeltaTime();

        MoveCamera();

        /* Draw Cubes Terrain (Ground) */

        cube.shader.Use();
        cube.shader.UniformMatrix("projection", projection.Pointer());
        cube.shader.UniformMatrix("view", view.Pointer());
        cube.shader.UniformVec4f("light_pos", lightPosition, 1);
        cube.shader.UniformVec4f("light_color", 1, 1, 1, 1);
        cube.shader.UniformFloat("brightness", 0.8);
        cube.shader.UnifomInteger("enable_blend", 0);
        cube.shader.UnifomInteger("enable_texture", 1);
        cube.va.Bind();
        cube.texture.Bind();


        /* Draw OBJ */

        cube.shader.UniformMatrix("model", FastMatrix::position_rotated(25, -1, 10, 180, 0, 1, 0));
        cube.shader.UnifomInteger("enable_texture", 1);
        
        stall.va.Bind();
        stall.texture.Bind();
        
        glDrawArrays(GL_TRIANGLES, 0, obj_loader->vertices.size() / 3);

        display.setTitle((char*)std::to_string(1 / display.delta_time).c_str());
        display.SwapBuffers();
    }

    Debugger::Log("Main", "User Closed Window");

    return 0;
}