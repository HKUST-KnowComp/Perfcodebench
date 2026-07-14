#pragma once

#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters);
