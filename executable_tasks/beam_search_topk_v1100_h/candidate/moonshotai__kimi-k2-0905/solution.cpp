#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

struct Entry {
    int32_t score;
    int     idx;
    bool operator<(const Entry& o) const noexcept {
        return score > o.score || (score == o.score && idx < o.idx);
    }
};

inline uint64_t mix(uint64_t h, uint64_t v) noexcept {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams, int vocab, int topk, int iters) {
    std::vector<Entry> buf;
    buf.reserve(static_cast<size_t>(vocab));

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);
            buf.clear();
            for (int v = 0; v < vocab; ++v) {
                buf.push_back({logits[base + static_cast<size_t>(v)], v});
            }
            std::nth_element(buf.begin(), buf.begin() + topk, buf.end());
            std::sort(buf.begin(), buf.begin() + topk);

            const Entry* top = buf.data();
            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k].score)));
                hash = mix(hash, static_cast<uint64_t>(top[k].idx));
            }
        }
    }
    return hash;
}