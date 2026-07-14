#pragma once

#include <cstdint>
#include <vector>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths);
