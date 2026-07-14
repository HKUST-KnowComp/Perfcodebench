#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes);
