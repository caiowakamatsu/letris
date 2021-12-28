#include "renderer.h"

let::renderer::renderer(std::uint16_t width, std::uint16_t height, let::opengl::manager *gl_manager) : _gl_manager(
        gl_manager), _resolution(width, height) {
    glGenFramebuffers(1, &_framebuffer.handle);

    glGenTextures(1, &_framebuffer.texture);
    glBindTexture(GL_TEXTURE_2D, _framebuffer.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, &_framebuffer.rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer.handle);

    glBindTexture(GL_TEXTURE_2D, _framebuffer.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _resolution.x, _resolution.y, 0, GL_RGBA, GL_FLOAT, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _framebuffer.texture, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, _framebuffer.rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _resolution.x, _resolution.y);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _framebuffer.rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LET_EXCEPTION(exception::source_type::render, "Framebuffer could not be completed");

    _triangle_program = _gl_manager->create_program({
        "shaders/triangle/shader.frag",
        "shaders/triangle/shader.vert"
    });

    _post_processing_programs.sky = _gl_manager->create_program({
        "shaders/post/sky/shader.comp"
    });

    auto vertices = std::array<float, 9>({
       -0.5, -0.5, -5.0,
        0.5, -0.5, -5.0,
        0.0,  0.5, -5.0
    });

    glGenBuffers(1, &_tri.vbo);
    glGenBuffers(1, &_tri.ebo);
    glGenVertexArrays(1, &_tri.vao);

    glBindVertexArray(_tri.vao);
    glBindBuffer(GL_ARRAY_BUFFER, _tri.vbo);

    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
}

std::uint32_t let::renderer::render(const renderer::render_data &data) {
    ZoneScopedN("renderer::render");
//    if (!data.vertices.empty())
//    {
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer.handle);
        glEnable(GL_DEPTH_TEST);
//        glEnable(GL_CULL_FACE);

//        glBindVertexArray(_tri.vao);
//        glBindBuffer(GL_ARRAY_BUFFER, _tri.vbo);

//        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), nullptr, GL_STATIC_DRAW);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
//        glEnableVertexAttribArray(0);

//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _tri.ebo);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * sizeof(uint32_t), nullptr, GL_STATIC_DRAW);

//        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _tri.vao);
//        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _tri.ebo);
//        glEnableVertexAttribArray(1);
//        glEnableVertexAttribArray(2);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _tri.ebo);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size_bytes(), data.indices.data(), GL_STATIC_DRAW);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto model = glm::mat4(1.0f);
        const auto view = glm::translate(glm::mat4(1.0f), -data.offset);
        const auto view_rot = data.rotation * view;

        const auto proj = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.f);
        const auto mvp = proj * view_rot * model;

        _gl_manager->bind(_triangle_program);
        _gl_manager->uniform("mvp", mvp);
//        glBindVertexArray(_tri.vao);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _tri.ebo);
        glDrawElements(GL_TRIANGLES, 0, GL_UNSIGNED_INT, nullptr);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Apply post processing
        _gl_manager->bind(_post_processing_programs.sky);

        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(data.rotation, scale, rotation, translation, skew, perspective);
        auto rotations = glm::vec2(glm::eulerAngles(rotation));

        _gl_manager->uniform("rotation", rotations);

        glDispatchCompute(glm::ceil(1920.0 / 32.0), glm::ceil(1080.0 / 32.0), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindImageTexture(0, _framebuffer.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
//    }

    return _framebuffer.texture;
}
