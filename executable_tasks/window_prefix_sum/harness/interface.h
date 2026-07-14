#pragma once

#include <cstdint>
#include <vector>

uint64_t window_sum_checksum(const std::vector<uint32_t>& values, int window, int iters);
