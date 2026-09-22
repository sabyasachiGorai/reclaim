#include <iostream>
#include <filesystem>

int main() {
    std::filesystem::path root = ".";

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(root)) {

        if (entry.is_regular_file()) {
            std::cout << "FILE: "
                      << entry.path()
                      << '\n';
        }
        else if (entry.is_directory()) {
            std::cout << "DIRECTORY: "
                      << entry.path()
                      << '\n';
        }
    }

    return 0;
}