#include "Format.h"

#include <cstdio>

std::string formatSize(std::uintmax_t bytes)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};

    double value = static_cast<double>(bytes);
    int unit = 0;

    while (value >= 1024.0 && unit < 4)
    {
        value /= 1024.0;
        ++unit;
    }

    char buffer[32];

    if (unit == 0)
        std::snprintf(buffer, sizeof(buffer), "%ju B", bytes);
    else
        std::snprintf(buffer, sizeof(buffer), "%.2f %s", value, units[unit]);

    return buffer;
}
