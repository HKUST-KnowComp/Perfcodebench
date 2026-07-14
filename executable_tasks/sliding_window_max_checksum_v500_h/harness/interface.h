#pragma once

#include <cstdint>
#include <vector>

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters);
