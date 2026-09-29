#include <iostream>
#include <fstream>
#include <vector>

#include "picosha2.h"

int main()
{
    std::ifstream file(
        "../test.txt",
        std::ios::binary
    );

    if (!file)
    {
        std::cerr << "Could not open file\n";
        return 1;
    }

    const std::size_t bufferSize = 8;

    std::vector<char> buffer(bufferSize);

    picosha2::hash256_one_by_one hasher;

    while (file)
    {
        file.read(buffer.data(), buffer.size());

        std::streamsize bytesRead = file.gcount();

        if (bytesRead > 0)
        {
            hasher.process(
                buffer.begin(),
                buffer.begin() + bytesRead
            );
        }
    }

    hasher.finish();

    std::vector<unsigned char> digest(
        picosha2::k_digest_size
    );

    hasher.get_hash_bytes(
        digest.begin(),
        digest.end()
    );

    std::cout << picosha2::bytes_to_hex_string(
        digest.begin(),
        digest.end()
    ) << '\n';

    return 0;
}