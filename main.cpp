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

#include <ft2build.h>
#include FT_FREETYPE_H

#define PRESSED 1
#define NOT_PRESSED 0

#include <unistd.h>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

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

FT_Library ft_lib;
FT_Face face;
FT_GlyphSlot slot;

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
    // load all obj models before making display to avoid displat overtime
    Debugger::Log("Game::Startup", "Loading OBJ Models before Initializing Display ...");
    if (! loadModels()) {
        Debugger::Error("Game::Startup", "Failed to load Models, models Loader error:\n\t" +
            LOAD_MODELS_ERROR);
            
        return false;
    }

    // Log 
    Debugger::Log("Game::Setup", "All OBJ Models Loaded");
    Debugger::Log("Game::Setup", "Making 512x512 Display ...");

    // make display from local function
    if (! MakeDisplay()) {
        Debugger::Error("Game::Setup", "Failed to Make Display, display manager error:\n\t"+
            MAKE_DISPLAY_ERROR);
            
        return false;
    }


    // add display pointer for keyboard and mouse
    Keyboard::setDisplay(&display);
    Mouse::setDisplay(&display);

    Debugger::Log("Game::Setup", "Making Stall Render Object ...");


    // make static unsigned int for static draw model
    unsigned int usage = GL_STATIC_DRAW;

    // init model va and add va layouts and buffers
    stall.va.Init();
    stall.va.AddLayout(3, stall_loader.vertices_pointer(), stall_loader.vertices_size * FLOAT_SIZE, usage);
    stall.va.AddLayout(2, stall_loader.uvs_pointer(), stall_loader.uvs_size * FLOAT_SIZE, usage);
    stall.va.AddLayout(3, stall_loader.normals_pointer(), stall_loader.normals_size * FLOAT_SIZE, usage);

    Debugger::Log("Game::Setup", "Making Static Shader ...");


    // load shader from resources
    if (! shader.loadFromFile(Res::get("static_shader.hlsl"))) {
        Debugger::Error("Game::Setup", "Failed to load Shader");
        return false;
    }

    // compile shader
    if (! shader.Compile()) {
        Debugger::Error("Game::Setup", "Failed to Compile Shader Program, more info:\n"
            + shader.error);
        return false;
    }


    // translate stall model and view
    //stall.model.Translate(0, 0, -10);    
    view.Translate(0, -1, -10);


    // set projection to "perspictive"
    projection.setProjection_Perspictive(60, 1, 0.1, 100);
    stall.texture = TextureBuffers::Make(45, 189, 2, 255, 512, 512);


    // init sphere va and add va layouts and buffers
    sphere.va.Init();
    sphere.va.AddLayout(3, sphere_loader.vertices_pointer(), sphere_loader.vertices_size * FLOAT_SIZE, usage);
    sphere.va.AddLayout(2, sphere_loader.uvs_pointer(), sphere_loader.uvs_size * FLOAT_SIZE, usage);
    sphere.va.AddLayout(3, sphere_loader.vertices_pointer(), sphere_loader.normals_size * FLOAT_SIZE, usage);

    glfwSetInputMode(display.glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    
    // make gameScene object and name it
    gameScene.setSceneName("main_scene");

    // add model and sphere to game scene
    gameScene.addRenderObject(makeSceneRenderObject("obj_model", stall.Pointer()));
    gameScene.addRenderObject(makeSceneRenderObject("sphere", sphere.Pointer()));


    // init Freetype library
    if (FT_Init_FreeType(&ft_lib)) {
        std::cout << "Failed to init FT Lib\n";
        exit(-1);
    }


    // init minecraft font face
    if(FT_New_Face(ft_lib, "res/fonts/Minecraft.ttf", 0, &face)) {
        std::cout << "Failed to load font\n";
        
    }

    slot = face->glyph;

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

bool ANDROID_CONTROLLER_LOOP = true;

namespace AndroidController {
    int horizontal = 0, vertical = 0, up = 0, down = 0, left = 0, right = 0, a = 0, b = 0, c = 0, d = 0;
}

void AndroidController_THREAD() {
	std::string from = "SensorController Thread";
	Debugger::Log(from, "Sensor Controller Thread");

	std::ifstream ifs;
	std::string dev_path =  "/home/mdhazim/FlaskProjects/AndroidController/dev";
	std::string line;

    float last_time = 0;

	while(ANDROID_CONTROLLER_LOOP) {
		// open file
		ifs.open(dev_path);

        std::getline(ifs, line);
        AndroidController::horizontal = std::atoi(line.c_str());

        std::getline(ifs, line);
        AndroidController::vertical = std::atoi(line.c_str());

        std::getline(ifs, line);
        AndroidController::up = std::atoi(line.c_str());

        std::getline(ifs, line);
        AndroidController::down = std::atoi(line.c_str());

        std::getline(ifs, line);
        AndroidController::right = std::atoi(line.c_str());

        std::getline(ifs, line);
        AndroidController::left = std::atoi(line.c_str());

        std::getline(ifs, line);
        AndroidController::a = std::atoi(line.c_str());

        std::getline(ifs, line);
        AndroidController::b = std::atoi(line.c_str());

        std::getline(ifs, line);
        AndroidController::c = std::atoi(line.c_str());

        std::getline(ifs, line);
        AndroidController::d = std::atoi(line.c_str());

        ifs.close();

        //printf("Hello\n");
	}
}


ActionManager EnableTextureAction, ChangeTranslateObject;
bool textureEnabled = false;
MatrixObject* translate_object;

struct Vec3 {
    float x, y, z;

    Vec3(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }  
};

GLenum gl_error;

bool checkGLError() {
    if ((gl_error = glGetError()) != GL_NO_ERROR){
        Debugger::Error("Check_GL_Error", "OpenGL Error : \n\tCode : " + std::to_string((int) gl_error));
        return false;
    }

    return true;
}

void Render_01() {
   	
}

void Render() {
    // Print Graphics Driver Information
    Debugger::Log("Driver Information", "OpenGL Version : " + std::string(display.opengl_Version()));
    Debugger::Log("Driver Information", "Graphics Renderer : " + std::string(display.graphicsRenderer()));
    Debugger::Log("Driver Information", "Graphics Vendor : " + std::string(display.graphicsVendor()));
    

    // Change light
    light.lightColor = Colors::GOLD;
	light.position.Translate(0, 0, 10);
    translate_object = &view;


    ActionManager changeCursorMode;

    // set names for actions
    EnableTextureAction.setName("enable_texture");
    ChangeTranslateObject.setName("change_translate_obj");
    changeCursorMode.setName("ccm");

    view.Translate(0, 0, 0);


    // make actions
    Actions actions;


    // add all actions to actions vector
    actions.addAction(EnableTextureAction.Pointer());
    actions.addAction(ChangeTranslateObject.Pointer());
    actions.addAction(changeCursorMode.Pointer());


    // make texture surface

    TextureSurface tex_surf;
    //tex_surf.setTexture(TextureBuffers::Make(255, 0, 0, 255, 64, 64));
    tex_surf.setTexture(Res::get("texs/opengl.jpg"));
    tex_surf.Make();

    //stall.model.Translate(0, 0, -20);

	std::thread AndroidControllerThread(AndroidController_THREAD);

    view.Translate(0, 0, -10);

    int cursorMode = GLFW_CURSOR_DISABLED;

    while(! display.shouldClose() && ! Keyboard::isPressed(GLFW_KEY_Q)) {
        Clear();


        // Increase all actions
        actions.IncreaseAll(display.delta_time);

        if (changeCursorMode.isAvailable(0.3) && Keyboard::isPressed(GLFW_KEY_TAB)) {
            cursorMode = (cursorMode == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            glfwSetInputMode(display.glfwWindow, GLFW_CURSOR, cursorMode);

            changeCursorMode.Zero();
        }


        // Move Translate Object By Keyboard Data

        //std::cout << AndroidController::horizontal << ", " << AndroidController::vertical << "\n";

        view.Translate(
            //-Keyboard::horizontal() * player_speed * display.delta_time,
			player_speed * display.delta_time * -AndroidController::horizontal,
            -(AndroidController::a?1:((AndroidController::b ? -1 : 0)))*player_speed*display.delta_time,
            player_speed * display.delta_time * AndroidController::vertical
        );


        float rotation_value = 0;

        if (AndroidController::c == PRESSED) rotation_value --;
        if (AndroidController::d == PRESSED) rotation_value ++;

        std::cout << "Rotation value : " << rotation_value << "\n";

        if (rotation_value != 0) stall.model.Rotate(
            rotation_speed * display.delta_time * rotation_value,
            0, 1, 0
        );


        // Rotate Model by mouse delta_x if user pressed mouse right button
        if (glfwGetMouseButton(display.glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE)) stall.model.Rotate(
            Mouse::delta_x * (rotation_speed / 2) * display.delta_time,
            0, 1, 0
        );


        // Reset model and light matrix
        if (glfwGetMouseButton(display.glfwWindow, GLFW_MOUSE_BUTTON_RIGHT)) {
            stall.model.Reset();
            light.position.Reset();
        } 


        // Disable or Enable Texture if user clicked T Button
        if (Keyboard::isPressed(GLFW_KEY_T) && EnableTextureAction.isAvailable(0.3)) {
            EnableTextureAction.Zero();
            textureEnabled = !textureEnabled;
        }

        // Disable Blend before drawing 
        glDisable(GL_BLEND);

        // Uniform Static Uniforms for shader
        shader.Use();
        shader.UniformMatrix("view",  view.Pointer());
        shader.UniformMatrix("projection", projection.Pointer());
        shader.UniformFloat("brightness", light.brightness);
        shader.UniformVec4f("light_pos", light.position.GLM_Vector(), 1);
        shader.UniformFloat("shine", 10);
        shader.UniformVec4f("light_color", light.lightColor);
        shader.Disable("enable_blend");
        shader.UniformInteger("enable_texture", (textureEnabled ? 1 : 0));

        // Bind Model texture and vertex array
        stall.texture.Bind(); 
        stall.va.Bind();
        
        // uniform model Matrix
        shader.UniformMatrix("model", stall.model.Pointer());

        // draw triangles arrays if model is drawable
        if (stall.Drawable) glDrawArrays(GL_TRIANGLES, 0, stall_loader.faces);

        // uniform sphere data
        shader.UniformMatrix("model", light.position.Pointer());
        shader.UniformVec4f("light_color", 1, 1, 1, 1);
        shader.UniformInteger("brightness", 100);
        shader.Disable("enable_texture");

        // bind shere vertex array
        sphere.va.Bind();

        // draw sphere if drawable
        if (sphere.Drawable) glDrawArrays(GL_TRIANGLES, 0, sphere_loader.faces);

        // Draw texture surface

        glDisable(GL_DEPTH_TEST);

        tex_surf.shader.Use();

        tex_surf.va.Bind();
        tex_surf.texture.Bind();

        tex_surf.model.Translate(display.delta_time * 10, 0, 0);

        tex_surf.shader.UniformMatrix("projection", glm::ortho(-5, 5, -5, 5));
        tex_surf.shader.UniformMatrix("position", tex_surf.model.Pointer());

        glDrawArrays(GL_QUADS, 0, 4);

        glEnable(GL_DEPTH_TEST);

        /* render frame buffer */
        RenderFrame();
    }

	ANDROID_CONTROLLER_LOOP = false;

	// Join Thread to avoid (abort)
	Debugger::Log("Render", "Waiting for Android controller Thread to close ...");
	AndroidControllerThread.join();
	Debugger::Log("Render", "Android Controller Thread closed");

	return;
}


int main(int c, char ** args) {
    
    //if (c > 1) model_path = std::string(args[1]);
    if (! Setup()) exit(-1);

    Debugger::Log("main", "Starting Main Loop ...");

    Render();

    Debugger::Log("main", "Game Main Loop Exit");

    exit(1);
}










