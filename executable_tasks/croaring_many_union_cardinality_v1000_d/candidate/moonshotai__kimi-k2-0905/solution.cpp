#include "interface.h"
#include <roaring/roaring.h>
#include <thread>
#include <vector>

namespace {

// Each thread adds its slice into the shared bitmap.
void add_range(roaring_bitmap_t* dst,
               const uint32_t* values,
               uint32_t begin,
               uint32_t end) {
    roaring_bitmap_add_many(dst, end - begin, values + begin);
}

} // anonymous namespace

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {

    if (offsets.size() < 2) return 0;

    roaring_bitmap_t* rb = roaring_bitmap_create();
    if (!rb) return 0;

    const std::size_t shards = offsets.size() - 1;
    const unsigned hardware = std::thread::hardware_concurrency();
    const unsigned threads_to_use = (shards < hardware) ? static_cast<unsigned>(shards) : hardware;

    if (threads_to_use <= 1) {
        // Single-thread fallback
        for (std::size_t s = 0; s < shards; ++s) {
            add_range(rb, values.data(), offsets[s], offsets[s + 1]);
        }
    } else {
        std::vector<std::thread> workers;
        workers.reserve(threads_to_use);

        const std::size_t block = (shards + threads_to_use - 1) / threads_to_use;
        for (unsigned t = 0; t < threads_to_use; ++t) {
            const std::size_t first = t * block;
            if (first >= shards) break;
            const std::size_t last = std::min(first + block, shards);

            workers.emplace_back([rb, &values, &offsets, first, last]() {
                for (std::size_t s = first; s < last; ++s) {
                    add_range(rb, values.data(), offsets[s], offsets[s + 1]);
                }
            });
        }
        for (auto& th : workers) th.join();
    }

    roaring_bitmap_run_optimize(rb);   // compress run containers
    roaring_bitmap_shrink_to_fit(rb);  // reclaim unused memory

    const uint64_t card = roaring_bitmap_get_cardinality(rb);
    roaring_bitmap_free(rb);
    return card;
}