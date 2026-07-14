#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    std::vector<std::pair<int32_t, int>> row(vocab);
    uint64_t hash = 0;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;

    auto cmp = [](const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
        return a.first > b.first || (a.first == b.first && a.second < b.second);
    };

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
            for (int v = 0; v < vocab; ++v) {
                row[v] = {logits[base + v], v};
            }

            std::nth_element(row.begin(), row.begin() + topk, row.end(), cmp);
            std::sort(row.begin(), row.begin() + topk, cmp);

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
