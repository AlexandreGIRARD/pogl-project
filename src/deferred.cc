#include "deferred.hh"

static uint buffer[4] = {GL_COLOR_ATTACHMENT0,
                         GL_COLOR_ATTACHMENT1,
                         GL_COLOR_ATTACHMENT2,
                         GL_COLOR_ATTACHMENT3};

Deferred::Deferred(int width, int height, shared_camera camera, bool width_shadow)
{
    // Color texture
    glGenTextures(1, &_colors);
    glBindTexture(GL_TEXTURE_2D, _colors);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Normal texture
    glGenTextures(1, &_normals);
    glBindTexture(GL_TEXTURE_2D, _normals);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Position texture
    glGenTextures(1, &_position);
    glBindTexture(GL_TEXTURE_2D, _position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Specular texture
    glGenTextures(1, &_specular);
    glBindTexture(GL_TEXTURE_2D, _specular);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // depth texture
    glGenTextures(1, &_depth);
    glBindTexture(GL_TEXTURE_2D, _depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // FBO for the g-buffer
    glGenFramebuffers(1, &_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colors, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _normals, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _position, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _specular, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth, 0);

    glDrawBuffers(4, buffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(-1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // output texture
    glGenTextures(1, &_output);
    glBindTexture(GL_TEXTURE_2D, _output);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // depth texture
    uint depth;
    glGenTextures(1, &depth);
    glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //FBO for final output
    glGenFramebuffers(1, &_final_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _final_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _output, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(-1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _program = program();
    _program.add_shader("deferred/deferred.vs.glsl", GL_VERTEX_SHADER);
    _program.add_shader("deferred/deferred.fs.glsl", GL_FRAGMENT_SHADER);
    _program.link();

    _boid = program();
    _boid.add_shader("deferred/deferred_boid.vs.glsl", GL_VERTEX_SHADER);
    _boid.add_shader("deferred/deferred.fs.glsl", GL_FRAGMENT_SHADER);
    _boid.link();

    _final = program();
    if (width_shadow) {
        _final.add_shader("main.vs.glsl", GL_VERTEX_SHADER);
        _final.add_shader("main.fs.glsl", GL_FRAGMENT_SHADER);
    }
    else {
        _final.add_shader("shadowless_main.vs.glsl", GL_VERTEX_SHADER);
        _final.add_shader("shadowless_main.fs.glsl", GL_FRAGMENT_SHADER);
    }
    _final.link();

    _camera = camera;
    _occlusion = std::make_shared<AmbientOcclusion>(width, height);

    set_screen_quad();
}

Deferred::~Deferred()
{}

void Deferred::gbuffer_render(shared_models models)
{
    _program.use();
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawBuffers(4, buffer);

    for (auto model : models)
        model->draw(_program);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Deferred::gbuffer_render(shared_models models, shared_swarms swarms)
{
    _program.use();
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawBuffers(4, buffer);

    for (auto model : models)
        model->draw(_program);

    _boid.use();
    for (auto swarm : swarms)
        swarm->draw(_boid);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Deferred::render()
{
    // occlusion rendering
    _occlusion->render(*this);

    _final.use();
    set_textures(_final);
    glBindFramebuffer(GL_FRAMEBUFFER, _final_FBO);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    render_screen_quad();
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Deferred::render_skybox(Skybox &skybox)
{
    // Copy depth buffer from _FBO to _final_FBO
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _final_FBO);

    glBlitFramebuffer(0, 0, 1920, 1080, 0, 0, 1920, 1080, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Render skybox in _final_FBO
    glBindFramebuffer(GL_FRAMEBUFFER, _final_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth, 0);
    skybox.render(*_camera);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Deferred::bind_fbo_to_backbuffer()
{
    // Output texture to backbuffer with depth test
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _final_FBO);
    glBlitFramebuffer(0, 0, 1920, 1080, 0, 0, 1920, 1080, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _FBO);
    glBlitFramebuffer(0, 0, 1920, 1080, 0, 0, 1920, 1080, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Deferred::set_screen_quad()
{
    float quad[] = {
        -1.f, -1.f, 0.f, 0.f, 0.f,
        -1.f,  1.f, 0.f, 0.f, 1.f,
         1.f, -1.f, 0.f, 1.f, 0.f,
         1.f,  1.f, 0.f, 1.f, 1.f
    };
    glGenVertexArrays(1, &_quad_VAO);
    glGenBuffers(1, &_quad_VBO);

    glBindVertexArray(_quad_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);

    // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);

    // Uvs
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3 * sizeof(float)));
}

void Deferred::render_screen_quad()
{
    glBindVertexArray(_quad_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Deferred::update_viewport()
{
    mat4 view = _camera->look_at();
    mat4 inverse_view = inverse(view);
    mat4 projection = _camera->get_projection();
    vec3 position = _camera->get_position();
    _program.use();
    _program.addUniformMat4(view, "view");
    _program.addUniformMat4(projection, "projection");
    _program.addUniformVec3(position, "cam_pos");
    _boid.use();
    _boid.addUniformMat4(view, "view");
    _boid.addUniformMat4(projection, "projection");
    _boid.addUniformVec3(position, "cam_pos");
    _final.use();
    _final.addUniformVec3(position, "cam_pos");
    _final.addUniformMat4(inverse_view, "inverse_view");
    _final.addUniformMat4(view, "view");
}

void Deferred::set_textures(program &p)
{
    p.addUniformTexture(0, "def.color");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _colors);

    p.addUniformTexture(1, "def.normal");
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, _normals);

    p.addUniformTexture(2, "def.position");
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, _position);

    p.addUniformTexture(3, "def.specular");
    glActiveTexture(GL_TEXTURE0+3);
    glBindTexture(GL_TEXTURE_2D, _specular);

    p.addUniformTexture(4, "def.occlusion");
    glActiveTexture(GL_TEXTURE0+4);
    glBindTexture(GL_TEXTURE_2D, _occlusion->get_map());
}

uint Deferred::get_pos()
{
    return _position;
}

uint Deferred::get_depth()
{
    return _depth;
}

uint Deferred::get_normal()
{
    return _normals;
}

program Deferred::get_final()
{
    return _final;
}

program Deferred::get_program()
{
    return _program;
}

uint Deferred::get_output()
{
    return _output;
}

Camera Deferred::get_camera()
{
    return *_camera;
}

void Deferred::set_shadow_maps(DirectionalLight &sun, std::vector<std::shared_ptr<PointLight>> lights)
{
    _final.use();
    sun.set_shadow_map(_final);
    for (auto light : lights)
        light->set_shadow_cube(_final);
}

void Deferred::set_camera(shared_camera camera)
{
    _camera = camera;
}
