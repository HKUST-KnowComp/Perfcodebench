#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint64_t transcode_checksum(const std::vector<char32_t>& input);
