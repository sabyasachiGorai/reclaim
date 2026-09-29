#pragma once

#include <string>
#include <vector>

#include "FileInfo.h"

struct DuplicateGroup {
    std::string hash;
    std::vector<FileInfo> files;
};