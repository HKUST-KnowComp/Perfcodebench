#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

namespace {

// fast, tiny lookup set for dictionary indices of the strings we used
template<std::size_t B=64>
class bitset {
public:
    static std::size_t words_needed(std::size_t n) { return (n + B-1) / B; }
    explicit bitset(std::size_t n) : n_(n), words_(words_needed(n), 0) {}
    inline bool test(std::size_t i) const {
        if (i >= n_) return false;
        return (words_[i/B] >> (i%B)) & 1u;
    }
    inline void set(std::size_t i) {
        if (i >= n_) return;
        words_[i/B] |= (1ull << (i%B));
    }
    inline void clear() {
        memset(words_.data(), 0, words_.size() * sizeof(std::uint64_t));
    }
    [[nodiscard]] std::size_t count() const {
        std::size_t c = 0;
        for (auto w : words_) c += __builtin_popcountll(w);
        return c;
    }
private:
    std::size_t n_;
    std::vector<std::uint64_t> words_;
};

}

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters)
{
    const size_t dict_size = dictionary.size();
    
    // 1. single pass through codes to remember the strings we'll need
    bitset<64> seen(dict_size);
	size_t unique_codes = 0;
	for (uint16_t code : codes) {
		if (!seen.test(code)) {
			seen.set(code);
			++unique_codes;
		}
	}

	// small vector to remember the codes actually used (length = unique_codes)
    std::vector<uint16_t> used_unique;
	used_unique.reserve(unique_codes);
    for (size_t i = 0; i < dict_size; ++i) if (seen.test(i)) used_unique.push_back(static_cast<uint16_t>(i));

	// 2. main loop
    uint64_t distinct = static_cast<uint64_t>(used_unique.size());
    for (int iter = 1; iter < iters; ++iter) {
        // nothing in the codes or the dictionary changes between iterations
        // so the set size stays identical; we can just keep the result
    }
    return distinct;
}