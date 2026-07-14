#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

namespace {
struct Entry {
    int32_t score;
    int id;
};

inline bool better(const Entry& a, const Entry& b) {
    if (a.score != b.score) return a.score > b.score;
    return a.id < b.id;
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}
} // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    std::vector<Entry> row(vocab);
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int32_t* beam_logits = &logits[b * vocab];
            for (int v = 0; v < vocab; ++v) {
                row[v] = {beam_logits[v], v};
            }
            
            std::nth_element(row.begin(), row.begin() + topk, row.end(), better);
            std::sort(row.begin(), row.begin() + topk, better);
            
            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].score)));
                hash = mix(hash, static_cast<uint64_t>(row[k].id));
            }
        }
    }
    return hash;
}