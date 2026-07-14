#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    std::vector<std::pair<int32_t, int>> row(vocab);
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    auto comp = [](const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
        return a.first > b.first || (a.first == b.first && a.second < b.second);
    };

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (int b = 0; b < beams; ++b) {
            const std::size_t base = static_cast<std::size_t>(b) * vocab;
            const int32_t* lptr = logits.data() + base;
            for (int v = 0; v < vocab; ++v) {
                row[v] = {lptr[v], v};
            }
            
            std::partial_sort(row.begin(), row.begin() + topk, row.end(), comp);
            
            for (int k = 0; k < topk; ++k) {
                hash ^= static_cast<uint64_t>(static_cast<uint32_t>(row[k].first));
                hash *= FNV_PRIME;
                hash ^= static_cast<uint64_t>(row[k].second);
                hash *= FNV_PRIME;
            }
        }
    }
    return hash;
}
