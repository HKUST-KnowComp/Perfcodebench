#include "interface.h"

#include <string_view>
#include <unordered_set>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int /*iters*/) {
    std::unordered_set<std::string_view> seen;
    for (auto code : codes) {
        seen.insert(dictionary[code]);
    }
    return static_cast<uint64_t>(seen.size());
}
