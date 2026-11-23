#include "Report.h"

#include <iostream>

#include "Format.h"

void printDuplicates(const std::vector<DuplicateGroup>& groups)
{
    std::uintmax_t totalWasted = 0;

    for (std::size_t i = 0; i < groups.size(); ++i)
    {
        const auto& group = groups[i];

        std::cout << "\nGroup " << (i + 1) << ": "
                  << group.files.size() << " copies, "
                  << formatSize(group.files[0].size) << " each, "
                  << formatSize(group.wastedBytes()) << " wasted\n";

        for (const auto& file : group.files)
            std::cout << "  " << file.path << '\n';

        totalWasted += group.wastedBytes();
    }

    std::cout << "\nDuplicate groups: " << groups.size()
              << "\nSpace that could be reclaimed: "
              << formatSize(totalWasted) << '\n';
}
