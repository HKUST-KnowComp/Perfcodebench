#include "interface.h"

uint64_t run_end_sum(const std::vector<uint32_t>& run_ends,
                     const std::vector<uint32_t>& values,
                     int iters) {
    const std::size_t n = values.size();
    // allocate once; resize with uninitialized values for speed
    static thread_local std::vector<uint32_t> run_len(n);
    run_len.resize(n);

    { uint32_t prev = 0;                 // compute run lengths once
      for (std::size_t i = 0; i < n; ++i) {
        run_len[i] = run_ends[i] - prev;
        prev = run_ends[i];
      }
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        for (std::size_t i = 0; i < n; ++i) {
            local += static_cast<uint64_t>(run_len[i]) *
                     static_cast<uint64_t>(values[i]);
        }
        sum = local;
    }
    return sum;
}