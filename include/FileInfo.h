#pragma once

#include <filesystem>
#include <cstdint>

struct FileInfo {
    std::filesystem::path path;
    std::uintmax_t size;
};