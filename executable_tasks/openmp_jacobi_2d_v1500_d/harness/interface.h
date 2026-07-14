#pragma once

#include <cstdint>
#include <vector>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps);
