#include "DuplicateDetector.h"
#include "Hasher.h"

std::unordered_map<
    std::uintmax_t,
    std::vector<FileInfo>
>
DuplicateDetector::groupBySize(
    const std::vector<FileInfo>& files
) {
    std::unordered_map<
        std::uintmax_t,
        std::vector<FileInfo>
    > groups;

    for (const auto& file : files)
    {
        groups[file.size].push_back(file);
    }

    return groups;
}

std::unordered_map<
    std::string,
    std::vector<FileInfo>
>
DuplicateDetector::groupByHash(
    const std::vector<FileInfo>& files
) {
    std::unordered_map<
        std::string,
        std::vector<FileInfo>
    > groups;

    Hasher hasher;

    for (const auto& file : files)
    {
        std::string hash =
            hasher.hashFile(file.path);

        if (!hash.empty())
        {
            groups[hash].push_back(file);
        }
    }

    return groups;
}

std::vector<DuplicateGroup>
DuplicateDetector::findDuplicates(
    const std::vector<FileInfo>& files
) {
    std::vector<DuplicateGroup> duplicates;

    auto sizeGroups =
        groupBySize(files);

    std::vector<FileInfo> candidates;

    for (const auto& [size, group] : sizeGroups)
    {
        if (group.size() > 1)
        {
            candidates.insert(
                candidates.end(),
                group.begin(),
                group.end()
            );
        }
    }

    auto hashGroups =
        groupByHash(candidates);

    for (const auto& [hash, group] : hashGroups)
    {
        if (group.size() > 1)
        {
            DuplicateGroup duplicateGroup;

            duplicateGroup.hash = hash;
            duplicateGroup.files = group;

            duplicates.push_back(
                duplicateGroup
            );
        }
    }

    return duplicates;
}