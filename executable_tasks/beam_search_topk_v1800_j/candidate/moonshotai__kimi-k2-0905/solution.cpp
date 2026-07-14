#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct Pair {
    int32_t score;
    int     index;
};

inline bool better(const Pair& a, const Pair& b) {
    return a.score > b.score || (a.score == b.score && a.index < b.index);
}

inline std::uint64_t mix(std::uint64_t hash, std::uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams,
                            int vocab,
                            int topk,
                            int iters)
{
    std::vector<Pair> heap;
    heap.reserve(static_cast<std::size_t>(topk));

    std::uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);

            heap.clear();
            for (int v = 0; v < vocab; ++v) {
                Pair cur{logits[base + v], v};
                if (heap.size() < static_cast<std::size_t>(topk)) {
                    heap.push_back(cur);
                    std::push_heap(heap.begin(), heap.end(), better);
                } else if (better(cur, heap.front())) {
                    std::pop_heap(heap.begin(), heap.end(), better);
                    heap.back() = cur;
                    std::push_heap(heap.begin(), heap.end(), better);
                }
            }

            std::sort_heap(heap.begin(), heap.end(), better);
            for (const Pair& p : heap) {
                hash = mix(hash, static_cast<std::uint64_t>(static_cast<std::uint32_t>(p.score)));
                hash = mix(hash, static_cast<std::uint64_t>(p.index));
            }
        }
    }
    return hash;
}