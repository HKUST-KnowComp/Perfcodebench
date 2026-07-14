#include "interface.h"
#include <vector>
#include <string_view>
#include <unordered_set>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (codes.empty()) return 0;

    // To avoid string copies, we use string_view which points to the existing dictionary strings.
    // We use a custom hash for string_view to ensure high performance.
    std::unordered_set<std::string_view> seen;
    seen.reserve(codes.size());

    uint64_t last_count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        seen.clear();
        for (uint16_t code : codes) {
            seen.insert(dictionary[static_cast<std::size_t>(code)]);
        }
        last_count = static_cast<uint64_t>(seen.size());
    }

    return last_count;
}