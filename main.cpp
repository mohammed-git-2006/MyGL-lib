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

class Game {
    public:
    Projection projection;
    MatrixObject view;

    OBJLoader stall_loader;

    Texture stall_texture;
    TextureSurface opengl_logo;

    Light light;

    float camera_speed = 20;
    float mouse_sensitivity = 1.5, player_speed = 10, rotation_speed = 100;

    RenderObject stall;

    ShaderProgram shader;

    std::string LOAD_MODELS_ERROR  = "No Errors",
                MAKE_DISPLAY_ERROR = "No Errors",
                LOAD_SHADER_ERROR  = "No Errors";

    bool loadModels() {
        Debugger::Log("loadModels", "Loading Stall Model ...");

        if (! stall_loader.loadFromFile(Res::get("dragon.obj"))) {
            LOAD_MODELS_ERROR = "Failed to load stall model";
            return false;
        }

        Debugger::Log("loadModels", "Stall Model Loaded");

        return true;
    }

    bool MakeDisplay() {
        if (! display.Make("GLFW window", 512, 512)) {
            MAKE_DISPLAY_ERROR = display.error;
            return false;
        }

        display.EnableBlend();
        display.EnableCullFace(GL_BACK);
        display.EnableDepth();

        display.setBackgroundColor(144, 144, 144, 255);

        return true;
    }

    bool Setup() {
        Debugger::Log("Game::Startup", "Loading OBJ Models before Initializing Display ...");
        if (! loadModels()) {
            Debugger::Error("Game::Startup", "Failed to load Models, models Loader error:\n\t" +
                this->LOAD_MODELS_ERROR);
            
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

        stall.va.Init();
        stall.va.AddLayout(
            3, 
            stall_loader.vertices_pointer(), 
            stall_loader.vertices_size * FLOAT_SIZE,
            GL_DYNAMIC_DRAW
        );

        stall.va.AddLayout(
            2, 
            stall_loader.uvs_pointer(), 
            stall_loader.uvs_size * FLOAT_SIZE,
            GL_DYNAMIC_DRAW
        );

        stall.va.AddLayout(
            3, 
            stall_loader.normals_pointer(), 
            stall_loader.normals_size * FLOAT_SIZE,
            GL_DYNAMIC_DRAW
        );

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

        view.Translate(0, 0, -10);

        projection.setProjection_Perspictive(60, 1, 0.1, 100);
        light.lightColor = Colors::WHITE;
        light.brightness = 1.5;
        stall.texture = CreateTexture(Res::get("stallTexture.png"));

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

    void Render() {
        while(! display.shouldClose() && ! Keyboard::isPressed(GLFW_KEY_Q)) {
            Clear();

            stall.model.Rotate(rotation_speed * display.delta_time, 0, 1, 0);
            
            view.Translate(
                -Keyboard::horizontal() * player_speed * display.delta_time,
                -(Keyboard::isPressed(GLFW_KEY_PAGE_UP) ? 1 : ( (Keyboard::isPressed(GLFW_KEY_PAGE_DOWN) ? -1 : 0) )) * player_speed * display.delta_time,
                Keyboard::vertical() * player_speed * display.delta_time
            );
            
            /* Render all */
            shader.Use();
            shader.UniformMatrix("view",  view.Pointer());
            shader.UniformMatrix("model", stall.model.Pointer());
            shader.UniformMatrix("projection", projection.Pointer());
            shader.UniformFloat("brightness", light.brightness);
            shader.UniformVec4f("light_pos", -view.x, -view.y, -view.z, 1);
            shader.UniformVec4f("light_color", light.lightColor);
            shader.Disable("enable_blend");
            shader.Disable("enable_texture");

            stall.texture.Bind();
            stall.va.Bind();

            glDrawArrays(GL_TRIANGLES, 0, stall_loader.faces);

            /* render frame buffer */

            RenderFrame();
        }
    }
};


int main(int c, char ** args) {
    Game game;
    
    if (! game.Setup()) exit(-1);

    Debugger::Log("main", "Starting Main Loop ...");

    game.Render();

    Debugger::Log("main", "Game Main Loop Exit");

    exit(1);
}

