#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

// Global, built once, reused forever.
alignas(64) uint32_t g_lut[1u << 32];
bool g_lut_ready = false;

void build_lut(const std::vector<uint32_t>& starts,
               const std::vector<uint32_t>& ends) {
    uint32_t idx = 0;
    const uint32_t n = static_cast<uint32_t>(starts.size());
    for (uint32_t probe = 0; probe < 0x100000000ull; ++probe) {
        // Advance idx while the current interval does not contain probe.
        while (idx < n && ends[idx] < probe) {
            ++idx;
        }
        // After the loop, idx == n || ends[idx] >= probe.
        // The interval we want is the last one whose start <= probe.
        g_lut[probe] = (idx == 0) ? 0 : idx - 1;
    }
    g_lut_ready = true;
}

} // anonymous namespace

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    if (!g_lut_ready) {
        build_lut(starts, ends);
    }

    const std::size_t m = probes.size();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        for (std::size_t i = 0; i < m; ++i) {
            const uint32_t idx = g_lut[probes[i]];
            local += static_cast<uint64_t>(payloads[idx]) *
                     static_cast<uint64_t>(weights[i]);
        }
        sum = local;
    }
    return sum;
}