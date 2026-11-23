#pragma once

#include <iostream>
#include <vector>

#include "DuplicateGroup.h"

// For each group, asks which copy to keep and deletes the others.
// Input and output are parameters so the flow can be tested.
void cleanDuplicates(const std::vector<DuplicateGroup>& groups,
                     std::istream& in,
                     std::ostream& out);
