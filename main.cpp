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
Projection projection;
MatrixObject view;
Position lightPosition;

OBJLoader stall_loader, dragon_loader;

Texture stall_texture;

float camera_speed = 10;

RenderObject dragon, stall;

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

void Load_OBJs() {
    Debugger::Log("Load_OBJs", "Loading Stall OBJ File ...");
    //stall_loader.loadFromFile("res/stall.obj");

    Debugger::Log("Load_OBJs", "Loading Dragon OBJ File, this will take a while becaise the model is too big ...");\
    dragon_loader.loadFromFile("res/dragon.obj");
}

int main(int c, char ** args) {
    Debugger::Log("Init", "Load_OBJs()");
    Load_OBJs();

    Debugger::Log("Init", "Initializing Display ...");
    display.Make("GLFW window", 700, 700);
    display.EnableBlend();
    display.EnableDepth();
    display.EnableCullFace(GL_BACK);

    GLFWimage* image = new GLFWimage;
    int comp;
    image->pixels = stbi_load("res/opengl.jpg", &image->width, &image->height, &comp, STBI_rgb_alpha);

    glfwSetWindowIcon(display.glfwWindow, 1, image);

    Debugger::Log("Init", "Initializing Keyboard and Mouse ...");
    Keyboard::setDisplay(&display);
    Mouse::setDisplay(&display);

    Debugger::Log("Init", "Initializing Projection ...");
    projection.setProjection_Perspictive(80, 1, 0.1, 200);

    float sky_diff = 0.05;

    //glClearColor(0.53 - sky_diff, 0.81 - sky_diff, 0.92 - sky_diff, 1);
    display.setBackgroundColor(0, 0, 0, 0);
    view.Translate(0, 0, -20);
    lightPosition.Move(2, 2, 0);

    Debugger::Log("Main", "Main Loop Entry");


    stall.va.Init(0, GL_QUADS);
    stall.va.AddLayout(3, &(stall_loader.vertices[0]),  stall_loader.vertices.size() * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    stall.va.AddLayout(2, &(stall_loader.texCoords[0]), stall_loader.texCoords.size() * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    stall.va.AddLayout(3, &(stall_loader.normals[0]),   stall_loader.normals.size() * FLOAT_SIZE, GL_DYNAMIC_DRAW);

    stall.texture = CreateTexture("res/stallTexture.png");

    stall.model.Translate(15, 0, 0);
    stall.model.matrix = glm::rotate(stall.model.matrix, glm::radians(180.0f), glm::vec3(0, 1, 0));

    dragon.va.Init(0, GL_QUADS);

    dragon.va.AddLayout(3, &(dragon_loader.vertices[0]),  dragon_loader.vertices.size() * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    dragon.va.AddLayout(2, &(dragon_loader.texCoords[0]), dragon_loader.texCoords.size() * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    dragon.va.AddLayout(3, &(dragon_loader.normals[0]),   dragon_loader.normals.size() * FLOAT_SIZE, GL_DYNAMIC_DRAW);

    ShaderProgram shader;
    shader.loadFromFile("res/cube_shader.hlsl");
    shader.Compile();

        
    Debugger::Log("Load_OBJs", "Loading Stall Texture ...");
    stall_texture.loadFromFile("res/stallTexture.png");

    while (! display.shouldClose() && !Keyboard::isPressed(GLFW_KEY_Q)) {
        display.Clear_Depth();
        display.Clear();
        display.calculateDeltaTime();

        MoveCamera();

        lightPosition.Move(-display.delta_time, 0, 0);

        dragon.model.Rotate(1, 0, 1, 0);

        shader.Use();
        shader.UniformMatrix("projection", projection.Pointer());
        shader.UniformMatrix("view", view.Pointer());
        //shader.UniformVec4f("light_pos", lightPosition, 1);
        shader.UniformVec4f("light_pos", -view.x, -view.y, -view.z, 1);
        shader.UniformVec4f("light_color", 1, 1, 1, 1);
        shader.UniformFloat("brightness", 0.8);
        shader.UniformInteger("enable_blend", 0);

        /* Draw OBJ */

        //shader.UniformMatrix("model", stall.model.Pointer());
        //shader.UniformInteger("enable_texture", 1);
        //
        //stall.va.Bind();
        //stall.texture.Bind();
        
        glDrawArrays(GL_TRIANGLES, 0, stall_loader.vertices.size() / 3);

        dragon.va.Bind();
        shader.UniformInteger("enable_texture", 0);
        shader.UniformMatrix("model", dragon.model.Pointer());
        
        glDrawArrays(GL_TRIANGLES, 0, dragon_loader.vertices.size() / 3);

        display.setTitle((char*)std::to_string(1 / display.delta_time).c_str());
        display.SwapBuffers();
    }

    Debugger::Log("Main", "User Closed Window");

    return 0;
}