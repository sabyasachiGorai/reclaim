#include <iostream>
#include <string>

#include "picosha2.h"

int main()
{
    const std::string input = "hello";

    std::string hash =
        picosha2::hash256_hex_string(input);

    std::cout << "SHA-256: "
              << hash
              << '\n';

    return 0;
}
