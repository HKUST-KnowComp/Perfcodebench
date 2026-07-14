#pragma once

#include <cstdint>
#include <vector>

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters);
