#pragma once

#include <cstdint>
#include <vector>

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters);
