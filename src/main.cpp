#include <iostream>
#include <filesystem>
#include <algorithm>

#include "Scanner.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: reclaim <directory>\n";
        return 1;
    }

    std::filesystem::path root = argv[1];

    std::error_code ec;

    if (!std::filesystem::exists(root, ec)) {
        if (ec) {
            std::cerr << "Error checking path: "
                      << ec.message()
                      << '\n';
        }
        else {
            std::cerr << "Error: path does not exist.\n";
        }

        return 1;
    }

    if (!std::filesystem::is_directory(root, ec)) {
        if (ec) {
            std::cerr << "Error checking directory: "
                      << ec.message()
                      << '\n';
        }
        else {
            std::cerr << "Error: path is not a directory.\n";
        }

        return 1;
    }

    Scanner scanner;

    std::vector<FileInfo> files = scanner.scan(root);

    std::sort(
        files.begin(),
        files.end(),
        [](const FileInfo& a, const FileInfo& b) {
            return a.size > b.size;
        }
    );

    const std::size_t topN = 5;

    const std::size_t count =
        std::min(topN, files.size());

    std::cout << "\nFiles found: "
              << files.size()
              << "\n\n";

    std::cout << "Top "
              << count
              << " largest files:\n\n";

    for (std::size_t i = 0; i < count; ++i) {
        std::cout << (i + 1)
                  << ". "
                  << files[i].path
                  << " | "
                  << files[i].size
                  << " bytes\n";
    }

    return 0;
}