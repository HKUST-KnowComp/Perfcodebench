#include "interface.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace {

constexpr uint64_t kOffset = 1469598103934665603ULL;
constexpr uint64_t kPrime  = 1099511628211ULL;

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= kPrime;
    return h;
}

struct Entry {
    int32_t score;
    int     idx;
};

// deterministic partial sort: keeps exactly the top-k elements
// (quick-select style, but stable tie-break via index)
inline void topk(Entry* __restrict buffer, int vocab, int k) {
    int left  = 0;
    int right = vocab - 1;
    while (true) {
        if (left + k - 1 >= right) break;

        // pivot = middle element
        int p = (left + right) >> 1;
        int32_t pivot = buffer[p].score;

        int i = left;
        int j = right;
        while (i <= j) {
            while (buffer[i].score > pivot || 
                   (buffer[i].score == pivot && buffer[i].idx < buffer[p].idx)) ++i;
            while (buffer[j].score < pivot || 
                   (buffer[j].score == pivot && buffer[j].idx > buffer[p].idx)) --j;
            if (i <= j) {
                std::swap(buffer[i], buffer[j]);
                ++i; --j;
            }
        }

        if (k <= i - left) {
            right = i - 1;
        } else {
            k -= (i - left);
            left = i;
        }
    }
    // final small sort for exact order among the k best
    std::sort(buffer, buffer + k,
              [](const Entry& a, const Entry& b) {
                  return a.score > b.score || (a.score == b.score && a.idx < b.idx);
              });
}

} // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams, int vocab, int topk, int iters) {
    uint64_t hash = kOffset;
    std::vector<Entry> scratch(static_cast<size_t>(vocab));

    for (int iter = 0; iter < iters; ++iter) {
        for (int b = 0; b < beams; ++b) {
            const int32_t* row = &logits[b * vocab];
            for (int v = 0; v < vocab; ++v) {
                scratch[v].score = row[v];
                scratch[v].idx   = v;
            }
            topk(scratch.data(), vocab, topk);
            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(scratch[k].score));
                hash = mix(hash, static_cast<uint64_t>(scratch[k].idx));
            }
        }
    }
    return hash;
}
