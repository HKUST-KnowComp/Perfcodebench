#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    std::vector<std::pair<int32_t, int>> top_k(topk);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int32_t* beam_logits = logits.data() + static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);

            for (int v = 0; v < topk; ++v) {
                top_k[v] = {beam_logits[v], v};
            }

            std::sort(top_k.begin(), top_k.end(), [](const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
                return a.first > b.first || (a.first == b.first && a.second < b.second);
            });

            int32_t worst_val = top_k.back().first;
            int worst_idx = top_k.back().second;

            for (int v = topk; v < vocab; ++v) {
                int32_t val = beam_logits[v];
                if (val > worst_val || (val == worst_val && v < worst_idx)) {
                    int pos = topk - 1;
                    while (pos > 0) {
                        int32_t p_val = top_k[pos-1].first;
                        int p_idx = top_k[pos-1].second;
                        if (val > p_val || (val == p_val && v < p_idx)) {
                            top_k[pos] = top_k[pos-1];
                            --pos;
                        } else {
                            break;
                        }
                    }
                    top_k[pos] = {val, v};
                    worst_val = top_k.back().first;
                    worst_idx = top_k.back().second;
                }
            }

            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top_k[k].first)));
                hash = mix(hash, static_cast<uint64_t>(top_k[k].second));
            }
        }
    }
    return hash;
}
