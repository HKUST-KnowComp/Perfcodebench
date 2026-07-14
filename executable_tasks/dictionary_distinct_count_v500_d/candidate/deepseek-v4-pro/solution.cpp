#include "interface.h"
#include <unordered_set>
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    (void)iters; // Unused parameter, distinct count does not depend on repetitions

    // Mark which codes are present using a bitset of size 65536 (max uint16_t value + 1)
    std::vector<bool> code_seen(65536, false);
    for (uint16_t c : codes) {
        code_seen[c] = true;
    }

    // Collect the distinct dictionary strings corresponding to seen codes
    std::unordered_set<std::string> distinct_strings;
    for (size_t i = 0; i < dictionary.size(); ++i) {
        if (code_seen[i]) {
            distinct_strings.insert(dictionary[i]);
        }
    }

    return static_cast<uint64_t>(distinct_strings.size());
}