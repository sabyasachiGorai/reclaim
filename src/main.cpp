#include <iostream>
#include <filesystem>
#include <algorithm>
#include <string>

#include "Cleaner.h"
#include "Format.h"
#include "Report.h"
#include "Scanner.h"
#include "DuplicateDetector.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: reclaim <directory> [--top N] [--clean] [--safe] [--dry-run]\n";
        return 1;
    }

    std::filesystem::path root = argv[1];
    std::size_t topN = 10;
    bool clean = false;
    CleanOptions options;

    for (int i = 2; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--top" && i + 1 < argc)
        {
            try
            {
                topN = std::stoul(argv[++i]);
            }
            catch (const std::exception &)
            {
                std::cerr << "Error: --top needs a number.\n";
                return 1;
            }
        }
        else if (arg == "--clean")
        {
            clean = true;
        }
        else if (arg == "--safe")
        {
            options.safe = true;
        }
        else if (arg == "--dry-run")
        {
            options.dryRun = true;
        }
        else
        {
            std::cerr << "Unknown argument: " << arg << '\n';
            return 1;
        }
    }

    std::error_code ec;

    if (!std::filesystem::exists(root, ec))
    {
        if (ec)
        {
            std::cerr << "Error checking path: "
                      << ec.message()
                      << '\n';
        }
        else
        {
            std::cerr << "Error: path does not exist.\n";
        }

        return 1;
    }

    if (!std::filesystem::is_directory(root, ec))
    {
        if (ec)
        {
            std::cerr << "Error checking directory: "
                      << ec.message()
                      << '\n';
        }
        else
        {
            std::cerr << "Error: path is not a directory.\n";
        }

        return 1;
    }

    options.reviewDir = root / "_duplicates_review";

    Scanner scanner;

    std::vector<FileInfo> files = scanner.scan(root);

    std::sort(
        files.begin(),
        files.end(),
        [](const FileInfo &a, const FileInfo &b)
        {
            return a.size > b.size;
        });

    const std::size_t count =
        std::min(topN, files.size());

    std::cout << "\nFiles found: "
              << files.size()
              << "\n\n";

    std::cout << "Top "
              << count
              << " largest files:\n\n";

    for (std::size_t i = 0; i < count; ++i)
    {
        std::cout << (i + 1)
                  << ". "
                  << files[i].path
                  << " | "
                  << formatSize(files[i].size)
                  << '\n';
    }

    DuplicateDetector detector;

    auto duplicates =
        detector.findDuplicates(files);
    printDuplicates(duplicates);

    if (clean && !duplicates.empty())
        cleanDuplicates(duplicates, options, std::cin, std::cout);

    return 0;
}