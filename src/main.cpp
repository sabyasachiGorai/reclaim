#include <iostream>
#include <filesystem>
#include <system_error>
#include <vector>

#include "FileInfo.h"

int main()
{
    std::filesystem::path root = ".";

    std::error_code ec;

    std::filesystem::recursive_directory_iterator it(
        root,
        std::filesystem::directory_options::skip_permission_denied,
        ec);

    std::filesystem::recursive_directory_iterator end;

    std::vector<FileInfo> files;

    while (it != end)
    {

        if (ec)
        {
            std::cerr << "Error while scanning: "
                      << ec.message()
                      << '\n';

            ec.clear();
        }

        const auto &entry = *it;

        if (entry.is_regular_file(ec))
        {

            FileInfo info;
            info.path = entry.path();
            info.size = std::filesystem::file_size(entry.path(), ec);

            if (ec)
            {
                std::cerr << "Could not get file size for: "
                          << entry.path()
                          << " - "
                          << ec.message()
                          << '\n';

                ec.clear();
            }
            else
            {
                files.push_back(info);
            }
        }
        else if (entry.is_directory(ec))
        {
            std::cout << "DIRECTORY: "
                      << entry.path()
                      << '\n';
        }

        it.increment(ec);
    }

std::cout << "\nFiles found: "
          << files.size()
          << "\n\n";

for (const auto& file : files) {
    std::cout << file.path
              << " | "
              << file.size
              << " bytes\n";
}

    return 0;
}