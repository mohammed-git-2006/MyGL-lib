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
#include "lib/TextureSurface.hpp"
#include "lib/ActionManager.hpp"
#include "lib/Color.hpp"
#include "lib/Light.hpp"

#include <unistd.h>

Display display;
Projection projection;
MatrixObject view;
Position lightPosition;

OBJLoader stall_loader, dragon_loader, camera_loader;

Texture stall_texture;
TextureSurface opengl_logo;

Light light;

float camera_speed = 10;

RenderObject dragon, stall, camera;

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
    stall_loader.loadFromFile("res/stall.obj");
    Debugger::Log("Load_OBJs", "Stall OBJ Loaded, Cleaning Temp ...");
    stall_loader.CleanTemp();

    //Debugger::Log("Load_OBJs", "Loading Dragon OBJ File, this will take a while because the model is too big ...");\
    //dragon_loader.loadFromFile("res/dragon.obj");
    //Debugger::Log("Load_OBJs", "Dragon OBJ Loaded, Cleaning Temp ...");
    //dragon_loader.CleanTemp();

    Debugger::Log("Load_OBJs", "Loading camera OBJ File ...");
    camera_loader.loadFromFile("res/camera.obj");
    Debugger::Log("Load_OBJs", "camera OBJ Loaded, Cleaning Temp ...");
    camera_loader.CleanTemp();
}

float dragon_brightness = 1;

void ScrollCallback(GLFWwindow* window, double x, double y) {
    dragon_brightness += 0.2 * y;

    Debugger::Log("ScrollCallback", "Dragon Brightness : " + std::to_string(dragon_brightness));
} 

int main(int c, char ** args) {
    Debugger::Log("Init", "Load_OBJs()");
    Load_OBJs();

    Debugger::Log("Init", "Initializing Display ...");
    display.Make("GLFW window", 700, 700);
    display.EnableBlend();
    display.EnableDepth();
    //display.EnableCullFace(GL_BACK);

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

    stall.va.Init(0, 0);
    stall.va.AddLayout(3, stall_loader.vertices_pointer(),  stall_loader.vertices_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    stall.va.AddLayout(2, stall_loader.uvs_pointer(), stall_loader.uvs_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    stall.va.AddLayout(3, stall_loader.normals_pointer(),   stall_loader.normals_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);

    stall.texture = CreateTexture("res/stallTexture.png");

    stall.model.Translate(15, 0, 0);
    stall.model.matrix = glm::rotate(stall.model.matrix, glm::radians(180.0f), glm::vec3(0, 1, 0));

    //dragon.va.Init(0, 0);
//
    //dragon.va.AddLayout(3, dragon_loader.vertices_pointer(),  dragon_loader.vertices_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    //dragon.va.AddLayout(2, dragon_loader.uvs_pointer(), dragon_loader.uvs_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    //dragon.va.AddLayout(3, dragon_loader.normals_pointer(),   dragon_loader.normals_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
//
    //dragon.model.Translate(0, 0, 0);

    camera.va.Init(0, 0);
    camera.va.AddLayout(3, camera_loader.vertices_pointer(), camera_loader.vertices_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    camera.va.AddLayout(2, camera_loader.uvs_pointer(), camera_loader.uvs_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    camera.va.AddLayout(3, camera_loader.normals_pointer(), camera_loader.normals_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);

    camera.model.Translate(0, 0, 0);

    ShaderProgram shader;
    shader.loadFromFile("res/cube_shader.hlsl");
    shader.Compile();

    opengl_logo.model.Translate(-4.5, 4.5, 0);
    opengl_logo.setTexture("res/opengl.jpg");
    opengl_logo.Make();

    std::cout << opengl_logo.texture.error << std::endl;

    Projection ui_projection;
    ui_projection.setProjection_Ortho(-5, 5, -5, 5);
    
    stall_texture.loadFromFile("res/stallTexture.png");

    Debugger::Log("Main", "Main Loop Entry");

    glfwSetScrollCallback(display.glfwWindow, ScrollCallback);

    ActionManager changeMouse_M;
    int mouse_disabled = false;


    light.lightColor = Colors::WHITE;
    light.lightDistance = 2;
    light.brightness = 1.2;

    while (! display.shouldClose() && !Keyboard::isPressed(GLFW_KEY_Q)) {
        display.Clear_Depth();
        display.Clear();
        display.calculateDeltaTime();

        changeMouse_M.Increase(display.delta_time);

        Mouse::Update();

        MoveCamera();

        if (Keyboard::isPressed(GLFW_KEY_TAB) && changeMouse_M.isAvailable(1)) {
            changeMouse_M.Zero();

            glfwSetInputMode(display.glfwWindow, GLFW_CURSOR, (mouse_disabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED));

            mouse_disabled = !mouse_disabled;
        }

        if (glfwGetMouseButton(display.glfwWindow, GLFW_MOUSE_BUTTON_LEFT))
            view.Rotate(Mouse::delta_x, 0, 1, 0);

        //stall.model.Rotate(1, 0, 1, 0);

        shader.Use();
        shader.UniformMatrix("projection", projection.Pointer());
        shader.UniformMatrix("view", view.Pointer());
        shader.UniformVec4f("light_pos", -view.x, -view.y, -view.z, 1);
        shader.UniformVec4f("light_color", light.lightColor);
        shader.UniformFloat("brightness", light.brightness);

        shader.Disable("enable_blend");

        /* Draw Stall */

        shader.UniformMatrix("model", stall.model.Pointer());
        shader.Enable("enable_texture");
        stall.va.Bind();
        stall.texture.Bind();
        
        glDrawArrays(GL_TRIANGLES, 0, stall_loader.vertices_size / 3);

        /* Draw Dragon */
        //dragon.va.Bind();
        //shader.UniformInteger("enable_texture", 0);
        //shader.UniformMatrix("model", dragon.model.Pointer());
        //
        //glDrawArrays(GL_TRIANGLES, 0, dragon_loader.vertices_size / 3);

        /* Draw camera */
        camera.va.Bind();
        shader.UniformMatrix("model", camera.model.Pointer());
        shader.Disable("enable_texture");
        
        glDrawArrays(GL_TRIANGLES, 0, camera_loader.vertices_size / 3);

        display.setTitle((char*)std::to_string(1 / display.delta_time).c_str());
        display.SwapBuffers();
    }

    Debugger::Log("Main", "User Closed Window");

    return 0;
}