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
    view.Translate(-25, -2, -40);
    lightPosition.Move(1, 1, 0);

    OBJLoader* obj_loader = new OBJLoader;
    obj_loader->loadFromFile("res/3d-model.obj");
    std::cout << "OBJ_Loader::error :\n\t" << obj_loader->error << std::endl;

    ShaderProgram obj_shader;
    obj_shader.loadFromFile("res/simple_shader.glsl");
    obj_shader.Compile();

    Texture obj_texture;
    obj_texture.loadFromFile("res/stallTexture.png");
    obj_texture.GenerateTexture();

    //Debugger::Log("Init", "Shader Compile Result : " + std::to_string(obj_shader.Compile()));

    std::cout << "Glm Vector Size : " << sizeof(glm::vec3) << std::endl;

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //int vertex_array_size = 5 * sizeof(float);
    int vertex_array_size = 0;

    unsigned int vbo, texcoordsBuffer, normalsBuffer;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * obj_loader->vertices.size(), &(obj_loader->vertices[0]), GL_DYNAMIC_DRAW);
    glGenBuffers(1, &texcoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texcoordsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * obj_loader->texCoords.size(), &(obj_loader->texCoords[0]), GL_DYNAMIC_DRAW);
    glGenBuffers(1, &normalsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * obj_loader->normals.size(), &(obj_loader->normals[0]), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_array_size, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, texcoordsBuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_array_size, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertex_array_size, nullptr);

    glBindVertexArray(0);

    std::cout << "obj_shader:\n\t" << obj_shader.error << std::endl;

    Debugger::Log("Init", "obj loader : temp_vertices : " + std::to_string(obj_loader->temp_vertices.size()));
    Debugger::Log("Main", "Main Loop Entry");

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

        //for(int x=0;x<50;x++) {
        //    for(int z=0;z<50;z++) {
        //        cube.shader.UniformMatrix("model", FastMatrix::position(x-2, -2, z-4));
        //        glDrawArrays(GL_QUADS, 0, 4 * 6);
        //    }
        //}


        /* Draw OBJ */

        cube.shader.UniformMatrix("model", FastMatrix::position_rotated(25, -1, 35, 180, 0, 1, 0));
        //cube.shader.UnifomInteger("enable_texture", 1);
        glBindVertexArray(vao);
        obj_texture.Bind();
        glDrawArrays(GL_TRIANGLES, 0, obj_loader->vertices.size() / 3);

        display.setTitle((char*)std::to_string(1 / display.delta_time).c_str());
        display.SwapBuffers();
    }

    Debugger::Log("Main", "User Closed Window");

    return 0;
}