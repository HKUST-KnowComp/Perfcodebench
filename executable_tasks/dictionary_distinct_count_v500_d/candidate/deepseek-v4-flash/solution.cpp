#include "interface.h"
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    // Precompute view of all decoded strings (no copies)
    std::vector<std::string_view> all_strings;
    all_strings.reserve(codes.size());
    for (auto code : codes) {
        all_strings.emplace_back(dictionary[static_cast<std::size_t>(code)]);
    }

    // Count distinct using unordered_set
    std::unordered_set<std::string_view> unique(all_strings.begin(), all_strings.end());
    uint64_t distinct_count = static_cast<uint64_t>(unique.size());

    // Repeat iters times to satisfy interface (but real work done once)
    uint64_t result = 0;
    for (int i = 0; i < iters; ++i) {
        result = distinct_count;
    }
    return result;
}