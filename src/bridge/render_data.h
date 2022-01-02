#ifndef LETRIS_RENDER_DATA_H
#define LETRIS_RENDER_DATA_H

#include <span>
#include <bitset>
#include <random>
#include <unordered_map>

#include <world/world.h>
#include <world/chunk/chunk.h>

#include <renderer/renderer.h>

#include <tracy/Tracy.hpp>

namespace let::bridge {

    class render_data;

    class render_data_cache {
    public:
        struct data
        {
            glm::ivec2 pos;
            std::array<std::optional<GLuint>, 16> vao;
            std::array<std::optional<GLuint>, 16> vbo;
            std::array<std::optional<GLuint>, 16> ebo;
            std::array<std::optional<GLuint>, 16> indirect;
        };

        explicit render_data_cache(opengl::manager *gl_manager);

        [[nodiscard]]
        std::optional<data> cached_chunk(uint64_t key) const noexcept;

    private:
        friend render_data;

        std::uint32_t _meshing_program;

        std::unordered_map<uint64_t, data> _chunks;

        let::opengl::manager *_gl_manager;
    };

    class render_data {
    public:
        explicit render_data(const let::world &world, render_data_cache &cache);

        struct chunk_data
        {
            std::vector<glm::ivec2> positions;
            std::vector<std::array<std::optional<GLuint>, 16>> vertices;
            std::vector<std::array<std::optional<GLuint>, 16>> indices;
            std::vector<std::array<std::optional<GLuint>, 16>> indirects;
        };
        [[nodiscard]] chunk_data data();

    private:
        std::unordered_map<uint64_t, render_data_cache::data> _chunks;

    };
}

#endif //LETRIS_RENDER_DATA_H