#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters);
