#include <iostream>
#include <filesystem>

int main() {
    std::filesystem::path currentPath =
        std::filesystem::current_path();

    std::cout << "Reclaim starting...\n";
    std::cout << "Current directory: "
              << currentPath.string()
              << '\n';

    return 0;
}