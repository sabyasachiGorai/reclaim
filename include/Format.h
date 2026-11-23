#pragma once

#include <cstdint>
#include <string>

// Turns a byte count into text like "1.50 MB".
std::string formatSize(std::uintmax_t bytes);
