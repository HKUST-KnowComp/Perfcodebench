#include "interface.h"
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    // Use a vector as a boolean map to track seen keys.
    // key_space is the upper bound of keys.
    std::vector<uint8_t> seen(key_space + 1, 0);
    std::vector<uint32_t> touched;
    touched.reserve(keys.size());

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        for (uint32_t key : keys) {
            if (seen[key] == 0) {
                seen[key] = 1;
                touched.push_back(key);
                count++;
            }
        }
        // Reset only the entries we touched to keep the next iteration fast
        for (uint32_t key : touched) {
            seen[key] = 0;
        }
        touched.clear();
    }
    return count;
}