#include "Hasher.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "picosha2.h"

std::string Hasher::hashFile(
    const std::filesystem::path &filePath)
{
    std::ifstream file(
        filePath,
        std::ios::binary);

    if (!file)
    {
        std::cerr << "Could not open file: "
                  << filePath
                  << '\n';

        return "";
    }

    const std::size_t bufferSize =
        8 * 1024 * 1024;

    std::vector<char> buffer(bufferSize);

    picosha2::hash256_one_by_one hasher;

    while (file)
    {
        file.read(
            buffer.data(),
            buffer.size());

        std::streamsize bytesRead =
            file.gcount();

        if (bytesRead > 0)
        {
            hasher.process(
                buffer.begin(),
                buffer.begin() + bytesRead);
        }
    }

    hasher.finish();

    std::vector<unsigned char> digest(
        picosha2::k_digest_size);

    hasher.get_hash_bytes(
        digest.begin(),
        digest.end());

    return picosha2::bytes_to_hex_string(
        digest.begin(),
        digest.end());
}