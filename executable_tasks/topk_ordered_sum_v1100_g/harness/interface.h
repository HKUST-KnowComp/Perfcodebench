#pragma once

#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters);
