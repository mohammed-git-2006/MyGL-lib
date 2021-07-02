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
#include "lib/Scene.hpp"

#include <unistd.h>

Display display;
Projection projection;
MatrixObject view;

OBJLoader stall_loader, sphere_loader;

Texture stall_texture;
TextureSurface opengl_logo;

Light light;

float camera_speed = 10;
float mouse_sensitivity = 1.5, player_speed = 10;

RenderObject stall, sphere;

void MoveCamera() {
    if (Keyboard::isPressed(GLFW_KEY_W))
        view.Translate(0, 0, 1 * display.delta_time * camera_speed);
    
    if (Keyboard::isPressed(GLFW_KEY_S))
        view.Translate(0, 0, -1 * display.delta_time * camera_speed);

    if (Keyboard::isPressed(GLFW_KEY_A))
        view.Translate(-1 * display.delta_time * camera_speed, 0, 0);

    if (Keyboard::isPressed(GLFW_KEY_D))
        view.Translate(1 * display.delta_time * camera_speed, 0, 0);

    if (Keyboard::isPressed(GLFW_KEY_PAGE_UP)) 
        view.Translate(0, -1 * display.delta_time * camera_speed, 0);

    if (Keyboard::isPressed(GLFW_KEY_PAGE_DOWN)) 
        view.Translate(0, 1 * display.delta_time * camera_speed, 0);
}

void Load_OBJs() {
    Debugger::Log("Load_OBJs", "Loading Stall OBJ File ...");
    std::cout << (stall_loader.loadFromFile("res/stall.obj") == true ? "Loaded" : "Failed")  << "\n";
    Debugger::Log("Load_OBJs", "Stall OBJ Loaded, Cleaning Temp ...");
    stall_loader.CleanTemp();

    Debugger::Log("Load_OBJs", "Loading Sphere OBJ File ...");
    sphere_loader.loadFromFile("res/sphere.obj");
    Debugger::Log("Load_OBJs", "Sphere OBJ Loaded, Cleaning Temp ...");
    sphere_loader.CleanTemp();
}

float dragon_brightness = 1;

void ScrollCallback(GLFWwindow* window, double x, double y) {
    dragon_brightness += 0.2 * y;

    Debugger::Log("ScrollCallback", "Dragon Brightness : " + std::to_string(dragon_brightness));
} 

glm::vec3 lookAt_Eye() {
    return glm::vec3(
        //sphere.model.x,
        //sphere.model.y + 2,
        sphere.model.x, sphere.model.y + 4,
        sphere.model.z + 10
    );

    //return glm::vec3(0, 0, 0);
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
    projection.setProjection_Perspictive(45, 1, 0.1, 200);

    float sky_diff = 0.05;

    display.setBackgroundColor(0, 0, 0, 0);
    view.Translate(0, -5, 0);

    stall.va.Init(0, 0);
    stall.va.AddLayout(3, stall_loader.vertices_pointer(),  stall_loader.vertices_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    stall.va.AddLayout(2, stall_loader.uvs_pointer(), stall_loader.uvs_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    stall.va.AddLayout(3, stall_loader.normals_pointer(),   stall_loader.normals_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    stall.texture = CreateTexture("res/stallTexture.png");
    stall.model.Translate(0, 0, -10);
    stall.model.matrix = glm::rotate(stall.model.matrix, glm::radians(180.0f), glm::vec3(0, 1, 0));
    stall_texture.loadFromFile("res/stallTexture.png");

    sphere.va.Init(0, 0);
    sphere.va.AddLayout(3, sphere_loader.vertices_pointer(), sphere_loader.vertices_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    sphere.va.AddLayout(2, sphere_loader.uvs_pointer(), sphere_loader.uvs_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    sphere.va.AddLayout(3, sphere_loader.normals_pointer(), sphere_loader.normals_size * FLOAT_SIZE, GL_DYNAMIC_DRAW);
    sphere.model.Translate(0, 0, 0);

    ShaderProgram shader;
    shader.loadFromFile("res/cube_shader.hlsl");
    shader.Compile();

    std::cout << opengl_logo.texture.error << std::endl;

    Projection ui_projection;
    ui_projection.setProjection_Ortho(-5, 5, -5, 5);


    Debugger::Log("Main", "Main Loop Entry");

    glfwSetScrollCallback(display.glfwWindow, ScrollCallback);

    ActionManager changeMouse_M, changeDrawMode_M;
    int mouse_disabled = false;
    GLenum draw_mode = GL_TRIANGLES;


    light.lightColor = Colors::WHITE;
    light.lightDistance = 2;
    light.brightness = 1.2;

    Debugger::Log("ProcessInfo", "Process ID : " + std::to_string(getpid()));

    while (! display.shouldClose() && !Keyboard::isPressed(GLFW_KEY_Q)) {
        display.Clear_Depth();
        display.Clear();
        display.calculateDeltaTime();

        changeMouse_M.Increase(display.delta_time);
        changeDrawMode_M.Increase(display.delta_time);

        Mouse::Update();

        if (Keyboard::isPressed(GLFW_KEY_R)) {
            sphere.model.Reset();
        }

        //MoveCamera();

        view.Translate(
            Mouse::delta_x * mouse_sensitivity * display.delta_time,
            Mouse::delta_y * mouse_sensitivity * display.delta_time,
            0
        );

        sphere.model.Translate(
            Keyboard::horizontal() * player_speed * display.delta_time,
            (Keyboard::isPressed(GLFW_KEY_PAGE_UP) ? 1 : (Keyboard::isPressed(GLFW_KEY_PAGE_DOWN) ? -1 : 0)) * 
                player_speed * display.delta_time,
            Keyboard::vertical() * -player_speed * display.delta_time
        );

        float radius = 15;

        view.matrix = glm::lookAt(
            //lookAt_Eye() ,
            //sphere.model.GLM_Vector(),
            //glm::vec3(0, 2, 0)
            glm::vec3(std::sin(view.x * 0.1) * radius + sphere.model.x, 0, std::cos(view.z) * radius + sphere.model.z),
            sphere.model.GLM_Vector(),
            glm::vec3(0, 2, 0)
        );

        printf("view.matrix = [");

        std::cout << view.matrix.length() << std::endl;

        for(int i=0;i<view.matrix.length();++i) {
            printf("%f", view.matrix[i]);

            if (i == (view.matrix.length() - 1)) printf("]\n");
            else printf(", ");
        }

        if (Keyboard::isPressed(GLFW_KEY_TAB) && changeMouse_M.isAvailable(1)) {
            changeMouse_M.Zero();

            glfwSetInputMode(display.glfwWindow, GLFW_CURSOR, (mouse_disabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED));

            mouse_disabled = !mouse_disabled;
        }

        if (Keyboard::isPressed(GLFW_KEY_V) && changeDrawMode_M.isAvailable(0.5)) {
            changeDrawMode_M.Zero();

            draw_mode = (draw_mode == GL_TRIANGLES ? GL_QUADS : GL_TRIANGLES);
        }

        //stall.model.Rotate(1, 0, 1, 0);

        shader.Use();
        shader.UniformMatrix("projection", projection.Pointer());
        shader.UniformMatrix("view", view.Pointer());
        //shader.UniformVec4f("light_pos", -view.x, -view.y, -view.z, 1);
        shader.UniformVec4f("light_pos", light.position.GLM_Vector(), 1);
        shader.UniformVec4f("light_color", light.lightColor);
        shader.UniformFloat("brightness", light.brightness);

        shader.Disable("enable_blend");

        /* Draw Stall */

        shader.UniformMatrix("model", stall.model.Pointer());
        shader.Enable("enable_texture");
        stall.va.Bind();
        stall.texture.Bind();
        
        glDrawArrays(draw_mode, 0, stall_loader.vertices_size / 3);

        /* Draw Sphere */
        sphere.va.Bind();
        shader.Disable("enable_texture");
        shader.UniformMatrix("model", sphere.model.Pointer());

        glDrawArrays(draw_mode, 0, sphere_loader.vertices_size / 3);

        display.setTitle((char*)std::to_string(1 / display.delta_time).c_str());
        display.SwapBuffers();
    }

    Debugger::Log("Main", "User Closed Window");

    return 0;
}