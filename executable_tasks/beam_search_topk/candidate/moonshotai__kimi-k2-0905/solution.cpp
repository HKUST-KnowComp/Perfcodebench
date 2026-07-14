#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <utility>

namespace {

inline bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
    return (a.first > b.first) || ((a.first == b.first) && (a.second < b.second));
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams,
                            int vocab,
                            int topk,
                            int iters) {
    // one-time scratch buffer reused across beams and iterations
    static thread_local std::vector<std::pair<int32_t, int>> buf;
    if (static_cast<int>(buf.size()) < vocab) buf.resize(static_cast<size_t>(vocab));

    const uint64_t init_hash = 1469598103934665603ULL;
    uint64_t hash = init_hash;

    for (int iter = 0; iter < iters; ++iter) {
        hash = init_hash;
        for (int b = 0; b < beams; ++b) {
            const int32_t* const ptr = &logits[static_cast<size_t>(b) * static_cast<size_t>(vocab)];

            // fill buffer
            for (int v = 0; v < vocab; ++v) {
                buf[static_cast<size_t>(v)] = {ptr[v], v};
            }

            // nth_element to partition top-k
            std::nth_element(buf.begin(), buf.begin() + topk, buf.begin() + vocab, better);

            // sort only the top-k slice for deterministic order
            std::sort(buf.begin(), buf.begin() + topk, better);

            // unrolled mixing
            const auto* top = buf.data();
            int k = 0;
            for (; k + 3 < topk; k += 4) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k].first)));
                hash = mix(hash, static_cast<uint64_t>(top[k].second));
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k+1].first)));
                hash = mix(hash, static_cast<uint64_t>(top[k+1].second));
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k+2].first)));
                hash = mix(hash, static_cast<uint64_t>(top[k+2].second));
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k+3].first)));
                hash = mix(hash, static_cast<uint64_t>(top[k+3].second));
            }
            for (; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k].first)));
                hash = mix(hash, static_cast<uint64_t>(top[k].second));
            }
        }
    }
    return hash;
}