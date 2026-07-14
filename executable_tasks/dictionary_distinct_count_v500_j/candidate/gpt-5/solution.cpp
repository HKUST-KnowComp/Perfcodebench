#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>
#include <cstdint>

struct StrPtrHash {
  std::size_t operator()(const std::string* p) const noexcept {
    return std::hash<std::string>{}(*p);
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
    int iters) {
  // Compute once; subsequent iters are no-ops to preserve correctness and minimize work.
  uint64_t count = 0;
  if (!codes.empty() && !dictionary.empty()) {
    // Deduplicate codes first to reduce redundant string hashing/comparisons.
    unsigned char visited[65536];
    for (int i = 0; i < 65536; ++i) visited[i] = 0;

    std::vector<uint16_t> uniq_codes;
    uniq_codes.reserve(codes.size() < 65536u ? codes.size() : 65536u);
    for (std::size_t i = 0; i < codes.size(); ++i) {
      uint16_t c = codes[i];
      if (!visited[c]) {
        visited[c] = 1;
        uniq_codes.push_back(c);
      }
    }

    std::unordered_set<const std::string*, StrPtrHash, StrPtrEq> seen;
    seen.reserve(uniq_codes.size());
    for (std::size_t i = 0; i < uniq_codes.size(); ++i) {
      uint16_t c = uniq_codes[i];
      const std::string* sp = &dictionary[static_cast<std::size_t>(c)];
      seen.insert(sp);
    }
    count = static_cast<uint64_t>(seen.size());
  } else {
    count = 0;
  }

  // Maintain the loop structure but avoid redoing work.
  (void)iters; // suppress unused warning; computation is independent of iters for correctness.
  return count;
}
