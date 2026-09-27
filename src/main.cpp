#include <iostream>
#include <filesystem>
#include <system_error>
#include <vector>

// #include "FileInfo.h"
#include <FileInfo.h>

int main(int argc, char* argv[])
{
    if(argc < 2){
        std::cerr << "Usage: reclaim <directory>\n";
        return 1;
    }
    std::filesystem::path root = argv[1];
    std::error_code ec;

    std::filesystem::recursive_directory_iterator it(root, std::filesystem::directory_options::skip_permission_denied, ec);

    std::filesystem::recursive_directory_iterator end;
    std::vector<FileInfo> files;
    while (it != end)
    {
        if (ec)
        {
            std::cerr << "Error while scnning: " << ec.message() << std::endl;
            ec.clear();
        }
        const auto &entry = *it;
        // entry.is_regular_file is same as it->is_directory are the same
        // auto& means, entry is a variable which shares the same memeory with (*it), but now you can accidently change it so we are using const
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
        else if (it->is_directory(ec))
        {
            std::cout << "DIRECTORY: " << entry.path() << std::endl;
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