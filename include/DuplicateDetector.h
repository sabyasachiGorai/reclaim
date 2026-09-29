#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "FileInfo.h"
#include "DuplicateGroup.h"

class DuplicateDetector {
public:
    std::unordered_map<
        std::uintmax_t,
        std::vector<FileInfo>
    > groupBySize(
        const std::vector<FileInfo>& files
    );

    std::unordered_map<
        std::string,
        std::vector<FileInfo>
    > groupByHash(
        const std::vector<FileInfo>& files
    );

    std::vector<DuplicateGroup> findDuplicates(
        const std::vector<FileInfo>& files
    );
};