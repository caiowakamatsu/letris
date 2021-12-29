#include "render_data.h"

let::bridge::render_data_cache::render_data_cache(opengl::manager *gl_manager) : _gl_manager(gl_manager) {
    _meshing_program = _gl_manager->create_program({
        "shaders/chunk_meshing/shader.comp"
    });
}

std::optional<let::bridge::render_data_cache::data> let::bridge::render_data_cache::cached_chunk(uint64_t key) const noexcept {
    auto it = _chunks.find(key);
    if (it == _chunks.end())
        return std::nullopt;
    else
        return it->second;
}

let::bridge::render_data::render_data(const let::world &world, render_data_cache &cache) {
    ZoneScopedN("render_data::render_data");

    // temporary, clear the chunks thing
    for (auto it : _chunks)
    {
        const auto data = it.second;
        glDeleteBuffers(3, reinterpret_cast<const GLuint *>(&data));
    }
    _chunks.clear();

    for (const auto &render_chunk : world._chunks)
    {
        cache._gl_manager->bind(cache._meshing_program);

        auto debug_block = let::block();
        debug_block.set_visible(block::face::up);
        debug_block.set_visible(block::face::down);

        cache._gl_manager->uniform("block", *debug_block.data());

        auto vao = GLuint();
        auto vbo = GLuint();
        auto ebo = GLuint();

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        _chunks[chunk::key(render_chunk.second)] =
            render_data_cache::data{
                    .vao = vao,
                    .vbo = vbo,
                    .ebo = ebo
            };

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), nullptr, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * sizeof(uint32_t), nullptr, GL_STATIC_DRAW);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ebo);

        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        break;
    }
}

let::bridge::render_data::chunk_data let::bridge::render_data::data() {
    auto chunks = chunk_data();
    chunks.vertices.reserve(_chunks.size());
    chunks.indices.reserve(_chunks.size());

    for (auto it : _chunks)
    {
        const auto data = it.second;
        chunks.vertices.push_back(data.vao);
        chunks.indices.push_back(data.ebo);
    }

    return chunks;
}
