#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

struct Token {
    int32_t score;
    int index;

    bool operator>(const Token& other) const {
        if (score != other.score) return score > other.score;
        return index < other.index;
    }
};

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    std::vector<Token> row(vocab);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const size_t base = static_cast<size_t>(b) * vocab;
            for (int v = 0; v < vocab; ++v) {
                row[v] = {logits[base + v], v};
            }

            if (topk < vocab) {
                std::nth_element(row.begin(), row.begin() + topk, row.end(), std::greater<Token>());
                std::sort(row.begin(), row.begin() + topk, std::greater<Token>());
            } else {
                std::sort(row.begin(), row.end(), std::greater<Token>());
            }

            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].score)));
                hash = mix(hash, static_cast<uint64_t>(row[k].index));
            }
        }
    }
    return hash;
}