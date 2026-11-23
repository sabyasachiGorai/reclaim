#include "Scanner.h"

#include <iostream>
#include <system_error>

std::vector<FileInfo> Scanner::scan(
    const std::filesystem::path& root
) {
    std::vector<FileInfo> files;

    std::error_code ec;

    std::filesystem::recursive_directory_iterator it(
        root,
        std::filesystem::directory_options::skip_permission_denied,
        ec
    );

    std::filesystem::recursive_directory_iterator end;

    while (it != end)
    {
        if (ec)
        {
            std::cerr << "Error while scanning: "
                      << ec.message()
                      << '\n';

            ec.clear();
        }

        const auto& entry = *it;

        // Don't re-scan files that --safe already moved aside.
        if (entry.path().filename() == "_duplicates_review")
        {
            it.disable_recursion_pending();
            it.increment(ec);
            continue;
        }

        // Skip symlinks so a link is never mistaken for a duplicate of its target.
        if (!entry.is_symlink(ec) && entry.is_regular_file(ec))
        {
            FileInfo info;

            info.path = entry.path();

            info.size = std::filesystem::file_size(
                entry.path(),
                ec
            );

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

        it.increment(ec);
    }

    return files;
}