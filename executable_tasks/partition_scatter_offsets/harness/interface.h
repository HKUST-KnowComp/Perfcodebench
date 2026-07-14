#pragma once

#include <cstdint>
#include <vector>

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters);
