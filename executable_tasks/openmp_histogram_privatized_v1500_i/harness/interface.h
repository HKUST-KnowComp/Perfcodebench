#pragma once

#include <cstdint>
#include <vector>

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters);
