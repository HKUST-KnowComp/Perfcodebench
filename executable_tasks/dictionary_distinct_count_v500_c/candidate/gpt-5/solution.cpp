#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>

namespace {
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
}  // namespace

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (iters <= 0) return 0;
  if (codes.empty()) return 0;

  // Deduplicate dictionary indices referenced by codes (uint16_t domain)
  std::vector<uint8_t> used(65536, 0);
  std::vector<uint16_t> unique_codes;
  unique_codes.reserve(codes.size());
  for (uint16_t c : codes) {
    if (!used[c]) {
      used[c] = 1;
      unique_codes.push_back(c);
    }
  }

  // Count unique string contents among referenced dictionary entries
  std::unordered_set<const std::string*, StrPtrHash, StrPtrEq> seen;
  seen.reserve(unique_codes.size());
  for (uint16_t idx : unique_codes) {
    const std::string* sp = &dictionary[static_cast<std::size_t>(idx)];
    seen.insert(sp);
  }

  return static_cast<uint64_t>(seen.size());
}
