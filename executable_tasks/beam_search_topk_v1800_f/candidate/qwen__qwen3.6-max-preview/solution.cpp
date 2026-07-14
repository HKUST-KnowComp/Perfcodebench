#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

namespace {

struct Entry {
    int32_t score;
    int32_t idx;
};

inline bool better(const Entry& a, const Entry& b) {
    return a.score > b.score || (a.score == b.score && a.idx < b.idx);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    std::vector<Entry> row(static_cast<std::size_t>(vocab));
    uint64_t hash = 0;
    const std::size_t topk_sz = static_cast<std::size_t>(topk);
    const std::size_t vocab_sz = static_cast<std::size_t>(vocab);

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const std::size_t base = static_cast<std::size_t>(b) * vocab_sz;
            for (std::size_t v = 0; v < vocab_sz; ++v) {
                row[v] = {logits[base + v], static_cast<int32_t>(v)};
            }
            
            if (topk_sz < vocab_sz) {
                std::nth_element(row.begin(), row.begin() + topk_sz, row.end(), better);
                std::sort(row.begin(), row.begin() + topk_sz, better);
            } else {
                std::sort(row.begin(), row.end(), better);
            }
            
            for (std::size_t k = 0; k < topk_sz; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].score)));
                hash = mix(hash, static_cast<uint64_t>(row[k].idx));
            }
        }
    }
    return hash;
}