#pragma once

#include <string>
#include <vector>

#include "FileInfo.h"

struct DuplicateGroup {
    std::string hash;
    std::vector<FileInfo> files;

    // Space freed if only one copy is kept.
    std::uintmax_t wastedBytes() const
    {
        return files.empty() ? 0 : files[0].size * (files.size() - 1);
    }
};