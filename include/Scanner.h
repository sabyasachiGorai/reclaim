#pragma once

#include <filesystem>
#include <vector>

#include "FileInfo.h"

class Scanner {
public:
    std::vector<FileInfo> scan(
        const std::filesystem::path& root
    );
};