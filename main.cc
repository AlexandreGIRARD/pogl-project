#include <iostream>
#include <cmath>
#include <filesystem>
#include <memory>

#include "util.hh"
#include "camera.hh"
#include "model.hh"
#include "directional_light.hh"
#include "point_light.hh"
#include "deferred.hh"
#include "water.hh"
#include "normal_material.hh"
#include "textured_material.hh"
#include "skybox.hh"

using namespace glm;

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    uint width = 1920;
    uint height = 1080;
    auto window = init_window(width, height);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        exit(-1);
    }

    // Debug
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug_callback, 0);

    // Tell OpenGL window size
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Enable z-buffer computation
    glEnable(GL_DEPTH_TEST);

    // Patch size = 3
    glPatchParameteri(GL_PATCH_VERTICES, 3);


    // Camera view and projection matrices
    Camera cam = Camera(vec3(0, 0, -5), vec3(1, 0, 0), vec3(0, 1, 0));
    mat4 view = cam.look_at();

    mat4 projection = mat4(1.0);
    projection = perspective(radians(90.0f), (float)width / (float)height, 1.f, 10.0f);

    // Add view projetcion to deferred shaders
    Deferred deferred = Deferred(width, height, true);

    // Set Skybox
    auto skybox = Skybox("textures/skybox/skybox", projection);

    // Sun Light init
    DirectionalLight sun = DirectionalLight(vec3(0, 0.5, -1), vec3(1, 1, 1), 1.f);
    sun.setup_program(vec3(0, 0, 0), vec3(0, 0.5, -1));
    sun.set_light_in_program(deferred.get_final());

    // Point Lights init
    std::vector<std::shared_ptr<PointLight>> lights;

    auto light2 = std::make_shared<PointLight>(vec3(0,0,3), vec3(1, 1, 1), 0.4f);
    light2->setup_program();
    light2->set_light_in_program(deferred.get_final());
    lights.emplace_back(light2);
    //
    // auto light = std::make_shared<PointLight>(vec3(0,0,-1), vec3(1, 1, 1), 0.4f);
    // light->setup_program();
    // light->set_light_in_program(deferred.get_final());
    // lights.emplace_back(light);

    // Material setting
    auto mat1 = std::make_shared<Normal_Material>(vec3(1,0,0), vec3(0.7,0.7,0.7), 0.25);
    auto mat2 = std::make_shared<Normal_Material>(vec3(0,1,1), vec3(0.7,0.7,0.7), 0.25);
    auto mat3 = std::make_shared<Normal_Material>(vec3(0.4,0.4,0.4), vec3(0.7,0.7,0.7), 0.25);
    auto tex = std::make_shared<Textured_Material>("textures/metal/");

    /************************   MODEL INIT   ***********************************/
    std::vector<std::shared_ptr<Model>> models;
    // Models matrix
    mat4 model = mat4(1.0);
    model = translate(model, vec3(2, -1, 2));
    model = scale(model, vec3(0.6, 0.6, 0.6));
    auto rad_off = 0.2f;

    auto teapot = std::make_shared<Model>("models/teapot_stanford.obj", model, mat1);
    models.emplace_back(teapot);

    model = mat4(1.0);
    model = translate(model, vec3(-2, -2, 2));
    auto cube = std::make_shared<Model>("models/smooth_sphere.obj", model, mat2);
    models.emplace_back(cube);

    model = mat4(1.0);

    auto model_trans = translate(model, vec3(0, -5, 0));
    auto model_scale = scale(model_trans, vec3(5, 5, 5));
    auto plane = std::make_shared<Model>("models/wall.obj", model_scale, mat3);
    models.emplace_back(plane);

    model_trans = translate(model, vec3(0, 0, 5));
    auto model_rotate = rotate(model_trans, radians(-90.f), vec3(1, 0, 0));
    model_scale = scale(model_rotate, vec3(5, 5, 5));
    plane = std::make_shared<Model>("models/wall.obj", model_scale, mat3);
    models.emplace_back(plane);

    model_trans = translate(model, vec3(0, -5, -5));
    model_rotate = rotate(model_trans, radians(90.f), vec3(1, 0, 0));
    model_scale = scale(model_rotate, vec3(5, 5, 5));
    plane = std::make_shared<Model>("models/wall.obj", model_scale, mat3);
    models.emplace_back(plane);

    model_trans = translate(model, vec3(-5, 0, 0));
    model_rotate = rotate(model_trans, radians(-90.f), vec3(0, 0, 1));
    model_scale = scale(model_rotate, vec3(5, 5, 5));
    plane = std::make_shared<Model>("models/wall.obj", model_scale, tex);
    models.emplace_back(plane);

    model_trans = translate(model, vec3(5, 0, 0));
    model_rotate = rotate(model_trans, radians(90.f), vec3(0, 0, 1));
    model_scale = scale(model_rotate, vec3(5, 5, 5));
    plane = std::make_shared<Model>("models/wall.obj", model_scale, tex);
    models.emplace_back(plane);
    /***************************************************************************/

    // Init water surface
    model_trans = translate(model, vec3(0, -1, 0));
    model_scale = scale(model_trans, vec3(5, 5, 5));
    auto water_surface = Model("models/wave.obj", model_scale, mat2);
    Water water = Water(width, height, water_surface, -1);
    water.setup_program(sun, lights);

    // Delta time setup
    double time = glfwGetTime();
    double delta = 0.0;

    // Mouse event setup
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double xpos, ypos;

    // Render loop
    while(!glfwWindowShouldClose(window))
    {
        // Callback quit window with KEY_ESCAPE
        quit_window(window);

        // Compute delta time
        delta = glfwGetTime() - time;
        time = glfwGetTime();
        // std::cout << time << std::endl;
        frame_rate(time);

        // Get mouse event (position variations)
        glfwGetCursorPos(window, &xpos, &ypos);

        // Update camera position
        cam.update(window, (float)delta*10, xpos, ypos);

        // Update sun position
        // sun.update_position(cam.get_position());

        // Update camera view and projection matrices
        mat4 view = cam.look_at();
        vec3 cam_pos = cam.get_position();

        // Update model matrices
        rad_off = pause_rotation(window, rad_off);
        auto teapot_model = teapot->get_model();
        teapot_model = rotate(teapot_model, radians(rad_off), vec3(0.0, 1.0, 0.0));
        teapot->set_model(teapot_model);

        auto cube_model = cube->get_model();
        // cube_model = rotate(cube_model, radians(-rad_off), vec3(1.0, 0.0, 0.0));
        cube->set_model(cube_model);


        // Deferred shading
        deferred.update_viewport(view, cam_pos);
        deferred.gbuffer_render(models);

        // Shadow computing
        sun.draw_shadow_map(models);
        for (auto light : lights)
            light->draw_shadow_map(models);

        // Render
        glViewport(0, 0, width, height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // deferred.set_textures(shaders);
        deferred.set_shadow_maps(sun, lights);
        deferred.render();
        deferred.render_skybox(skybox, view);
        deferred.bind_fbo_to_backbuffer();

        // skybox.render(view);
        water.render(models, cam, deferred, skybox);

        // Check and call events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Clear bufffer
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glfwTerminate();
    return 0;
}
