#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>
#include <utility>

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    std::vector<std::pair<int32_t, int>> row(vocab);
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int32_t* beam_logits = logits.data() + static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
            for (int v = 0; v < vocab; ++v) {
                row[v] = {beam_logits[v], v};
            }
            
            std::partial_sort(row.begin(), row.begin() + topk, row.end(), [](const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
                return a.first > b.first || (a.first == b.first && a.second < b.second);
            });
            
            for (int k = 0; k < topk; ++k) {
                hash ^= static_cast<uint32_t>(row[k].first);
                hash *= 1099511628211ULL;
                hash ^= static_cast<uint64_t>(row[k].second);
                hash *= 1099511628211ULL;
            }
        }
    }
    return hash;
}