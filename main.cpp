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
#include "lib/Resources.hpp"

#include <unistd.h>

Display display;

Projection projection;
MatrixObject view;
OBJLoader stall_loader, sphere_loader;
Texture stall_texture;
TextureSurface opengl_logo;
Light light;
float camera_speed = 20;
float mouse_sensitivity = 1.5, player_speed = 10, rotation_speed = 50;
RenderObject stall, sphere;
ShaderProgram shader;

Scene gameScene;

std::string LOAD_MODELS_ERROR  = "No Errors",
            MAKE_DISPLAY_ERROR = "No Errors",
            LOAD_SHADER_ERROR  = "No Errors";


//std::string model_path = "/home/mdhazim/Blender/untitled.obj";
std::string model_path = "res/dragon.obj";

bool loadModels() {
    Debugger::Log("loadModels", "Loading Stall Model ...");

    if (! stall_loader.loadFromFile(model_path)) {
        LOAD_MODELS_ERROR = stall_loader.error;
        return false;
    }

    Debugger::Log("loadModels", "Stall Model Loaded");
    Debugger::Log("loadModels", "Loading Sphere Model ...");

    if (! sphere_loader.loadFromFile(Res::get("sphere.obj"))) {
        LOAD_MODELS_ERROR = sphere_loader.error;
        return false;
    }

    Debugger::Log("loadModels", "Sphere Model Loaded");

    return true;
}


void Scroll_Callback(GLFWwindow* window, double x, double y) {
    view.Translate(0, 0, player_speed * display.delta_time * y * 10);
}

bool MakeDisplay() {
    if (! display.Make("GLFW window", 600, 600)) {
        MAKE_DISPLAY_ERROR = display.error;
        return false;
    }

    display.EnableBlend();
    display.EnableDepth();
	display.EnableCullFace(GL_BACK);

    display.setBackgroundColor(135, 206, 235, 255);

    glfwSetScrollCallback(display.glfwWindow, Scroll_Callback);

    return true;
}


bool Setup() {
    Debugger::Log("Game::Startup", "Loading OBJ Models before Initializing Display ...");
    if (! loadModels()) {
        Debugger::Error("Game::Startup", "Failed to load Models, models Loader error:\n\t" +
            LOAD_MODELS_ERROR);
            
        return false;
    }

    Debugger::Log("Game::Setup", "All OBJ Models Loaded");
    Debugger::Log("Game::Setup", "Making 512x512 Display ...");

    if (! MakeDisplay()) {
        Debugger::Error("Game::Setup", "Failed to Make Display, display manager error:\n\t"+
            MAKE_DISPLAY_ERROR);
            
        return false;
    }

    Keyboard::setDisplay(&display);
    Mouse::setDisplay(&display);

    Debugger::Log("Game::Setup", "Making Stall Render Object ...");

    unsigned int usage = GL_STATIC_DRAW;

    stall.va.Init();
    stall.va.AddLayout(3, stall_loader.vertices_pointer(), stall_loader.vertices_size * FLOAT_SIZE, usage);
    stall.va.AddLayout(2, stall_loader.uvs_pointer(), stall_loader.uvs_size * FLOAT_SIZE, usage);
    stall.va.AddLayout(3, stall_loader.normals_pointer(), stall_loader.normals_size * FLOAT_SIZE, usage);

    Debugger::Log("Game::Setup", "Making Static Shader ...");

    if (! shader.loadFromFile(Res::get("static_shader.hlsl"))) {
        Debugger::Error("Game::Setup", "Failed to load Shader");
        return false;
    }

    if (! shader.Compile()) {
        Debugger::Error("Game::Setup", "Failed to Compile Shader Program, more info:\n"
            + shader.error);
        return false;
    }

    stall.model.Translate(0, 0, 0);    
    view.Translate(0, -1, -10);

    projection.setProjection_Perspictive(60, 1, 0.1, 100);
    light.lightColor = Colors::WHITE;
    //light.lightColor = Colors::White(150);
    light.brightness = 0.8;
    stall.texture = CreateTexture(Res::get("texs/stallTexture.png"));

    sphere.va.Init();
    sphere.va.AddLayout(3, sphere_loader.vertices_pointer(), sphere_loader.vertices_size * FLOAT_SIZE, usage);
    sphere.va.AddLayout(2, sphere_loader.uvs_pointer(), sphere_loader.uvs_size * FLOAT_SIZE, usage);
    sphere.va.AddLayout(3, sphere_loader.vertices_pointer(), sphere_loader.normals_size * FLOAT_SIZE, usage);

    glfwSetInputMode(display.glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    gameScene.setSceneName("main_scene");

    gameScene.addRenderObject(makeSceneRenderObject("obj_model", stall.Pointer()));
    gameScene.addRenderObject(makeSceneRenderObject("sphere", sphere.Pointer()));


    return true;
}


void Clear() {
    display.Clear_Depth();
    display.calculateDeltaTime();

    Mouse::Update();
}


void RenderFrame() {
    display.SwapBuffers();
}


ActionManager EnableTextureAction, ChangeTranslateObject;
bool textureEnabled = true;
MatrixObject* translate_object;


void Render_01() {
    VertexArray quad;

    std::vector<glm::vec3> vertices {
        glm::vec3(-0.5, -0.5, 0),
        glm::vec3(-0.5, 0.5, 0),
        glm::vec3(-0.5, 0.5, 0),
    };

    quad.Init();
    //quad.AddLayout(3, &vertices[0].x, vertices.size() * VEC_SIZE, GL_STATIC_DRAW);
    glGenBuffers(1, &quad.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, quad.VBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, nullptr);
    
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * VEC_SIZE, &vertices[0].x, GL_STATIC_DRAW);

    //quad.LoadElementArrayBuffer(&indices[0], 6, GL_STATIC_DRAW);

    while(! display.shouldClose() && !Keyboard::isPressed(GLFW_KEY_Q)) {
        Clear();

        quad.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
      

        RenderFrame();
    }
}

void Render() {
    Debugger::Log("Driver Information", "OpenGL Version : " + std::string(display.opengl_Version()));
    Debugger::Log("Driver Information", "Graphics Renderer : " + std::string(display.graphicsRenderer()));
    Debugger::Log("Driver Information", "Graphics Vendor : " + std::string(display.graphicsVendor()));
    
    //stall.model.Rotate(45, 0, 1, 0);
    //stall.model.Scale(1.5, 1.5, 1.5);

    light.lightColor = Colors::GOLD;
	light.position.Translate(0, 0, 25);

    translate_object = &view;

    EnableTextureAction.setName("enable_texture");
    ChangeTranslateObject.setName("change_translate_obj");

    Actions actions;

    actions.addAction(EnableTextureAction.Pointer());
    actions.addAction(ChangeTranslateObject.Pointer());

    while(! display.shouldClose() && ! Keyboard::isPressed(GLFW_KEY_Q)) {
        Clear();

        actions.IncreaseAll(display.delta_time);

        if (actions["change_translate_obj"]->isAvailable(0.5) && Keyboard::isPressed(GLFW_KEY_C)) {
            actions["change_translate_obj"]->Zero();
            translate_object = (translate_object == &light.position ? &view : &light.position);
        }

        translate_object->Translate(
            -Keyboard::horizontal() * player_speed * display.delta_time,
            -(Keyboard::isPressed(GLFW_KEY_PAGE_UP) ? 1 : ( (Keyboard::isPressed(GLFW_KEY_PAGE_DOWN) ? -1 : 0) )) * player_speed * display.delta_time,
            Keyboard::vertical() * player_speed * display.delta_time
        );

		stall.model.Rotate(display.delta_time * rotation_speed, 0, 1, 0);

        if (glfwGetMouseButton(display.glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE)) stall.model.Rotate(
            Mouse::delta_x * (rotation_speed / 2) * display.delta_time,
            0, 1, 0
        );

        if (glfwGetMouseButton(display.glfwWindow, GLFW_MOUSE_BUTTON_RIGHT)) {
            stall.model.Reset();
            light.position.Reset();
            //view.Reset();
        } 

        if (Keyboard::isPressed(GLFW_KEY_T) && EnableTextureAction.isAvailable(0.3)) {
            EnableTextureAction.Zero();
            textureEnabled = !textureEnabled;
        }

        glDisable(GL_BLEND);

        /* Render all */
        //shader.UniformVec4f("light_pos", 0, 0, 1, 1);
        shader.Use();
        shader.UniformMatrix("view",  view.Pointer());
        shader.UniformMatrix("projection", projection.Pointer());
        shader.UniformFloat("brightness", light.brightness);
        shader.UniformVec4f("light_pos", light.position.GLM_Vector(), 1);
        shader.UniformFloat("shine", 10);
        shader.UniformVec4f("light_color", light.lightColor);
        shader.Disable("enable_blend");
        shader.UniformInteger("enable_texture", (textureEnabled ? 1 : 0));
        stall.texture.Bind();
        stall.va.Bind();
        
        shader.UniformMatrix("model", stall.model.Pointer());

        if (stall.Drawable) glDrawArrays(GL_TRIANGLES, 0, stall_loader.faces);
        //glDrawElements(GL_TRIANGLES, 0, GL_UNSIGNED_INT, nullptr);

        shader.UniformMatrix("model", light.position.Pointer());
        shader.UniformVec4f("light_color", 1, 1, 1, 1);
        shader.UniformInteger("brightness", 100);
        shader.Disable("enable_texture");

        sphere.va.Bind();

        if (sphere.Drawable) glDrawArrays(GL_TRIANGLES, 0, sphere_loader.faces);

        glEnable(GL_BLEND);

        /* render frame buffer */
        RenderFrame();
    }
}


int main(int c, char ** args) {
    
    //if (c > 1) model_path = std::string(args[1]);
    if (! Setup()) exit(-1);

    Debugger::Log("main", "Starting Main Loop ...");

    Render();

    Debugger::Log("main", "Game Main Loop Exit");

    exit(1);
}
