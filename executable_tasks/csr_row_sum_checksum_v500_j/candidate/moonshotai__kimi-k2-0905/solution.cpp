#include "interface.h"
#include <vector>
#include <numeric>
#include <execution>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters)
{
    const std::size_t num_rows = row_ptr.size() - 1;
    const uint64_t init_hash = 1469598103934665603ULL;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = init_hash;

        // Heuristic: parallel only when enough rows to amortise overhead
        if (num_rows >= 4096) {
            std::vector<uint64_t> partial(num_rows);
            std::for_each(std::execution::par,
                          partial.begin(), partial.end(),
                          [&](uint64_t& out) {
                              std::size_t r = &out - partial.data();
                              const uint32_t* beg = values.data() + row_ptr[r];
                              const uint32_t* end = values.data() + row_ptr[r + 1];
                              uint64_t sum = 0;
                              // 4-way unroll
                              const std::size_t len = end - beg;
                              std::size_t i = 0;
                              for (; i + 3 < len; i += 4) {
                                  sum += static_cast<uint64_t>(beg[i]);
                                  sum += static_cast<uint64_t>(beg[i+1]);
                                  sum += static_cast<uint64_t>(beg[i+2]);
                                  sum += static_cast<uint64_t>(beg[i+3]);
                              }
                              for (; i < len; ++i) sum += static_cast<uint64_t>(beg[i]);
                              out = sum;
                          });
            for (uint64_t s : partial) hash = mix(hash, s);
        } else {
            for (std::size_t r = 0; r < num_rows; ++r) {
                const uint32_t* beg = values.data() + row_ptr[r];
                const uint32_t* end = values.data() + row_ptr[r + 1];
                uint64_t sum = 0;
                const std::size_t len = end - beg;
                std::size_t i = 0;
                for (; i + 3 < len; i += 4) {
                    sum += static_cast<uint64_t>(beg[i]);
                    sum += static_cast<uint64_t>(beg[i+1]);
                    sum += static_cast<uint64_t>(beg[i+2]);
                    sum += static_cast<uint64_t>(beg[i+3]);
                }
                for (; i < len; ++i) sum += static_cast<uint64_t>(beg[i]);
                hash = mix(hash, sum);
            }
        }
    }
    return hash;
}