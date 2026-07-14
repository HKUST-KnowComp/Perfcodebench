#include "interface.h"

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

struct StringPtrHash {
    std::size_t operator()(const std::string* s) const noexcept {
        return std::hash<std::string>()(*s);
    }
};

struct StringPtrEqual {
    bool operator()(const std::string* a, const std::string* b) const noexcept {
        return *a == *b;
    }
};

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    (void)iters;  // unused; count is invariant over iterations

    std::unordered_set<const std::string*, StringPtrHash, StringPtrEqual> seen;
    seen.reserve(codes.size());

    for (auto code : codes) {
        seen.insert(&dictionary[static_cast<std::size_t>(code)]);
    }

    return static_cast<uint64_t>(seen.size());
}
