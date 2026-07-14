#include "interface.h"

#include <string>
#include <vector>

namespace {
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;
}

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  // Precompute per-dictionary token attributes to avoid touching string data repeatedly
  const std::size_t dict_size = dict.size();
  std::vector<uint64_t> dict_len(dict_size);
  std::vector<uint64_t> dict_first(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dict[i];
    dict_len[i] = static_cast<uint64_t>(s.size());
    dict_first[i] = s.empty() ? 0u : static_cast<unsigned char>(s[0]);
  }

  // Precompute per-code attributes for sequential access during iterations
  const std::size_t m = codes.size();
  std::vector<uint64_t> code_len(m);
  std::vector<uint64_t> code_first(m);
  for (std::size_t i = 0; i < m; ++i) {
    const std::size_t idx = static_cast<std::size_t>(codes[i]);
    code_len[i] = dict_len[idx];
    code_first[i] = dict_first[idx];
  }

  uint64_t hash = 0;  // Preserve baseline behavior: if iters == 0, return 0
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV_OFFSET_BASIS;
    const uint64_t* lenp = code_len.data();
    const uint64_t* firstp = code_first.data();
    for (std::size_t i = 0; i < m; ++i) {
      h ^= lenp[i];
      h *= FNV_PRIME;
      h ^= firstp[i];
      h *= FNV_PRIME;
    }
    hash = h;
  }
  return hash;
}
