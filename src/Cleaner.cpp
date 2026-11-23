#include "Cleaner.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <string>
#include <system_error>

#include "Format.h"

namespace fs = std::filesystem;

namespace
{
std::string modifiedTime(const fs::path& path)
{
    std::error_code ec;
    auto ftime = fs::last_write_time(path, ec);

    if (ec)
        return "unknown";

    // C++17 has no clock_cast, so convert through "now" on both clocks.
    auto sys = std::chrono::system_clock::now() +
               (ftime - fs::file_time_type::clock::now());
    std::time_t t = std::chrono::system_clock::to_time_t(
        std::chrono::time_point_cast<std::chrono::system_clock::duration>(sys));

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", std::localtime(&t));
    return buffer;
}

// Returns 1-based choice, 0 for skip, -1 if input ended.
int askWhichToKeep(std::size_t count, std::istream& in, std::ostream& out)
{
    std::string line;

    while (true)
    {
        out << "Keep which one? (1-" << count << " / 's' to skip): ";

        if (!std::getline(in, line))
            return -1;

        if (line == "s" || line == "S")
            return 0;

        try
        {
            std::size_t used = 0;
            int choice = std::stoi(line, &used);

            if (used == line.size() && choice >= 1 &&
                static_cast<std::size_t>(choice) <= count)
                return choice;
        }
        catch (const std::exception&)
        {
            // fall through to the error message
        }

        out << "Invalid input.\n";
    }
}
} // namespace

void cleanDuplicates(const std::vector<DuplicateGroup>& groups,
                     std::istream& in,
                     std::ostream& out)
{
    std::uintmax_t freed = 0;

    for (std::size_t g = 0; g < groups.size(); ++g)
    {
        const auto& files = groups[g].files;

        out << "\nGroup " << (g + 1) << " of " << groups.size() << ":\n";

        for (std::size_t i = 0; i < files.size(); ++i)
            out << "  [" << (i + 1) << "] " << files[i].path
                << "  (modified " << modifiedTime(files[i].path) << ")\n";

        int keep = askWhichToKeep(files.size(), in, out);

        if (keep < 0)
            break; // input ended
        if (keep == 0)
            continue;

        for (std::size_t i = 0; i < files.size(); ++i)
        {
            if (static_cast<int>(i) + 1 == keep)
                continue;

            std::error_code ec;
            fs::remove(files[i].path, ec);

            if (ec)
            {
                out << "  Could not delete " << files[i].path << ": "
                    << ec.message() << '\n';
            }
            else
            {
                out << "  Deleted " << files[i].path << '\n';
                freed += files[i].size;
            }
        }
    }

    out << "\nSpace reclaimed: " << formatSize(freed) << '\n';
}
