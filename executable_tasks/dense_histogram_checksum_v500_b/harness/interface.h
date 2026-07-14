#pragma once

#include <cstdint>
#include <vector>

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters);
