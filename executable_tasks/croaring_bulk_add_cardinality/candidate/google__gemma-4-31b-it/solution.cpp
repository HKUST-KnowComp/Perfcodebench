#include "interface.h"
#include <roaring/roaring.h>
#include <algorithm>

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
    if (values.empty()) {
        return 0;
    }

    // Since the input is explicitly stated as sorted in the task description,
    // the fastest way to count unique elements is to use std::unique.
    // This avoids the overhead of building a bitmap entirely.
    
    // We create a copy to avoid mutating the input vector if it were non-const,
    // but since it is const, we can't use std::unique in-place.
    // However, we can simply iterate and count transitions.
    
    uint64_t unique_count = 1;
    for (size_t i = 1; i < values.size(); ++i) {
        if (values[i] != values[i - 1]) {
            unique_count++;
        }
    }
    
    return unique_count;
}