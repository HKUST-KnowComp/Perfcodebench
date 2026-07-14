#pragma once

#include <cstdint>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters);
