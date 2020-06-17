#pragma once

#include "program.hh"
#include "model.hh"
#include "camera.hh"
#include "point_light.hh"
#include "directional_light.hh"

using shared_model = std::shared_ptr<Model>;
using shared_light = std::shared_ptr<PointLight>;

class Water
{
public:
    Water(int width, int height, Model &water_surface, float y);
    void setup_program(DirectionalLight sun_light, std::vector<shared_light> lights);
    void render(std::vector<shared_model> models, Camera cam);

private:
    Model _water_surface;
    program _water;
    program _middle;
    uint _reflection_FBO;
    uint _refraction_FBO;
    uint _reflection_tex;
    uint _refraction_tex;

    vec4 _clip_reflection;
    vec4 _clip_refraction;

    int _width;
    int _height;
    float _y;
};
