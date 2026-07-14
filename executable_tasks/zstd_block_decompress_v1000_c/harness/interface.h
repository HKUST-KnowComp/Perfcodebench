#pragma once

#include <cstdint>
#include <string>

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size);
