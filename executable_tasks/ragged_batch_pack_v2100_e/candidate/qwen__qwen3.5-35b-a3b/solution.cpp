#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    size_t total_elements = 0;
    for (int b = 0; b < batch; ++b) {
        total_elements += lengths[b];
    }

    std::vector<uint32_t> packed_tokens;
    std::vector<uint32_t> positions;
    packed_tokens.reserve(total_elements);
    positions.reserve(total_elements);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        packed_tokens.clear();
        positions.clear();
        size_t idx = 0;
        for (int b = 0; b < batch; ++b) {
            int len = lengths[b];
            size_t base = static_cast<size_t>(b) * static_cast<size_t>(max_len);
            for (int i = 0; i < len; ++i) {
                packed_tokens[idx] = padded_tokens[base + i];
                positions[idx] = static_cast<uint32_t>(i);
                idx++;
            }
        }
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < total_elements; ++i) {
            hash = mix(hash, packed_tokens[i]);
            hash = mix(hash, positions[i]);
        }
    }
    return hash;
}