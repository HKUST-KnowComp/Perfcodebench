#include "interface.h"
#include <algorithm>
#include <vector>

namespace {
    bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
        return a.first > b.first || (a.first == b.first && a.second < b.second);
    }

    uint64_t mix(uint64_t hash, uint64_t value) {
        hash ^= value;
        hash *= 1099511628211ULL;
        return hash;
    }
}

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    const std::size_t vocab_sz = static_cast<std::size_t>(vocab);
    const std::size_t topk_sz = static_cast<std::size_t>(topk);
    const std::size_t k_limit = std::min(topk_sz, vocab_sz);
    
    std::vector<std::pair<int32_t, int>> row(vocab_sz);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int32_t* base_ptr = logits.data() + static_cast<std::size_t>(b) * vocab_sz;
            auto* row_data = row.data();
            for (std::size_t v = 0; v < vocab_sz; ++v) {
                row_data[v] = {base_ptr[v], static_cast<int>(v)};
            }
            std::nth_element(row.begin(), row.begin() + k_limit, row.end(), better);
            std::sort(row.begin(), row.begin() + k_limit, better);
            for (std::size_t k = 0; k < k_limit; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row_data[k].first)));
                hash = mix(hash, static_cast<uint64_t>(row_data[k].second));
            }
        }
    }
    return hash;
}
