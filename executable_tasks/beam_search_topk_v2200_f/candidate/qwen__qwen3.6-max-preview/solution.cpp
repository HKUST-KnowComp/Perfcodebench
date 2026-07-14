#include "interface.h"

#include <algorithm>
#include <vector>
#include <cstdint>

namespace {

struct Element {
    int32_t score;
    int32_t idx;
};

inline bool better(const Element& a, const Element& b) {
    return a.score > b.score || (a.score == b.score && a.idx < b.idx);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    std::vector<Element> row(vocab);
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int32_t* beam_logits = logits.data() + static_cast<std::size_t>(b) * vocab;
            for (int v = 0; v < vocab; ++v) {
                row[v] = {beam_logits[v], v};
            }
            
            if (topk < vocab) {
                std::nth_element(row.begin(), row.begin() + topk, row.end(), better);
                std::sort(row.begin(), row.begin() + topk, better);
            } else {
                std::sort(row.begin(), row.end(), better);
            }
            
            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint32_t>(row[k].score));
                hash = mix(hash, static_cast<uint32_t>(row[k].idx));
            }
        }
    }
    return hash;
}
