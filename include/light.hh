#ifndef LIGHT_HH
#define LIGHT_HH

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "model.hh"
#include "program.hh"
#include "camera.hh"

using namespace glm;

class Light
{
public:
    Light(vec3 color, float intensity);
    virtual uint set_shadow_framebuffer() = 0;
    virtual void draw_shadow_map(std::vector<std::shared_ptr<Model>> models) = 0;
    // virtual void setup_program(vec3 direction, vec3 optional_pos) = 0;
    virtual void set_light_in_program(program p) = 0;

    uint get_light_id();
    uint get_map();
    mat4 get_projection();
    mat4 get_view();
    program get_program();

    static uint _nb_lights;

protected:
    vec3 _pos;
    vec3 _color;
    float _intensity;

    mat4 _projection;
    mat4 _view;

    uint _id;
    uint _FBO;
    uint _map;
    program _program;
};

#endif /* LIGHT_HH */
