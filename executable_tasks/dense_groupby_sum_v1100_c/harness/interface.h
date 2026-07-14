#pragma once

#include <cstdint>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters);
