#ifndef LETRIS_ULTRALIGHT_FILESYSTEM_H
#define LETRIS_ULTRALIGHT_FILESYSTEM_H

#include <AppCore/AppCore.h>

#include <fstream>
#include <filesystem>
#include <daw/daw_memory_mapped_file.h>

#include <tracy/Tracy.hpp>

namespace let {
    class ultralight_filesystem : public ultralight::FileSystem {
    private:
        struct loaded_file {
            daw::filesystem::memory_mapped_file_t<uint8_t> file;
            std::string path;
        };
        std::unordered_map<size_t, loaded_file> _files;

    public:

        [[nodiscard]] bool FileExists(const ultralight::String16 &path) override;

        [[nodiscard]] bool GetFileSize(ultralight::FileHandle handle, int64_t &result) override;

        [[nodiscard]] bool GetFileMimeType(const ultralight::String16 &path, ultralight::String16 &result) override;

        [[nodiscard]] ultralight::FileHandle OpenFile(const ultralight::String16 &path, bool open_for_writing) override;

        [[nodiscard]] int64_t ReadFromFile(ultralight::FileHandle handle, char *data, int64_t length) override;

        void CloseFile(ultralight::FileHandle &handle) override;
    };
}

#endif
