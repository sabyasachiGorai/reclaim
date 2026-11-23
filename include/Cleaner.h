#pragma once

#include <filesystem>
#include <iostream>
#include <vector>

#include "DuplicateGroup.h"

struct CleanOptions
{
    bool safe = false;   // move to a review folder instead of deleting
    bool dryRun = false; // only print what would happen
    std::filesystem::path reviewDir; // used when safe is true
};

// For each group, asks which copy to keep and deletes the others.
// Input and output are parameters so the flow can be tested.
void cleanDuplicates(const std::vector<DuplicateGroup>& groups,
                     const CleanOptions& options,
                     std::istream& in,
                     std::ostream& out);
