#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>
#include <cstdint>

struct StrPtrHash {
  size_t operator()(const std::string* s) const noexcept {
    return std::hash<std::string>{}(*s);
  }
};

struct StrPtrEq {
  bool operator()(const std::string* a, const std::string* b) const noexcept {
    return *a == *b;
  }
};

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int /*iters*/) {
  if (codes.empty()) return 0;

  // Step 1: Deduplicate code indices using a fixed-size bitset for 16-bit space.
  // 65536 bits => 1024 uint64_t chunks.
  static constexpr size_t BITSET_WORDS = 65536 / 64; // 1024
  uint64_t seen_bits[BITSET_WORDS];
  for (size_t i = 0; i < BITSET_WORDS; ++i) seen_bits[i] = 0ULL;

  std::vector<uint16_t> unique_codes;
  unique_codes.reserve(codes.size() < 65536u ? codes.size() : 65536u);

  for (uint16_t c : codes) {
    const uint32_t idx = static_cast<uint32_t>(c);
    const uint32_t word = idx >> 6;        // idx / 64
    const uint32_t bit = idx & 63u;        // idx % 64
    const uint64_t mask = 1ULL << bit;
    uint64_t w = seen_bits[word];
    if ((w & mask) == 0) {
      seen_bits[word] = w | mask;
      unique_codes.push_back(c);
    }
  }

  // Step 2: Count distinct decoded strings using pointer keys with content-based hash/eq.
  std::unordered_set<const std::string*, StrPtrHash, StrPtrEq> distinct;
  distinct.reserve(unique_codes.size());

  for (uint16_t code : unique_codes) {
    // Assuming valid indices as per task contract.
    const std::string* sp = &dictionary[static_cast<size_t>(code)];
    distinct.insert(sp);
  }

  return static_cast<uint64_t>(distinct.size());
}
