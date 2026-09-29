#pragma once

#include <filesystem>
#include <string>

class Hasher {
public:
    std::string hashFile(
        const std::filesystem::path& filePath
    );
};