#include "interface.h"
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    // Use a vector of bytes as a boolean array to track seen keys.
    // This avoids hashing overhead and memory allocation during iterations.
    std::vector<uint8_t> seen(key_space + 1, 0);
    std::vector<uint32_t> seen_indices;
    seen_indices.reserve(keys.size());

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        seen_indices.clear();
        for (uint32_t key : keys) {
            if (seen[key] == 0) {
                seen[key] = 1;
                seen_indices.push_back(key);
                count++;
            }
        }
        // Reset only the entries we touched to keep the next iteration fast
        for (uint32_t idx : seen_indices) {
            seen[idx] = 0;
        }
    }
    return count;
}