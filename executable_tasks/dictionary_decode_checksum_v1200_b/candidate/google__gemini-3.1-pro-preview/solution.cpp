#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct DictEntry {
  uint64_t size;
  uint64_t first_char;
};

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  
  if (iters <= 0) return 0;

  // Pre-calculate dictionary metadata to avoid string object overhead and improve cache locality.
  // We store only the size and the first character (or 0 if empty).
  std::vector<DictEntry> prepared_dict;
  prepared_dict.reserve(dict.size());
  for (const auto& s : dict) {
    prepared_dict.push_back({
        static_cast<uint64_t>(s.size()),
        s.empty() ? 0ULL : static_cast<uint64_t>(static_cast<unsigned char>(s[0]))
    });
  }

  uint64_t hash = 0;
  const size_t num_codes = codes.size();
  const uint32_t* codes_ptr = codes.data();
  const DictEntry* dict_ptr = prepared_dict.data();

  // The checksum calculation is repeated 'iters' times.
  // We avoid materializing the decoded vector entirely.
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_codes; ++i) {
      const DictEntry& entry = dict_ptr[codes_ptr[i]];
      // Inline the mix logic for the two components of each token.
      hash ^= entry.size;
      hash *= 1099511628211ULL;
      hash ^= entry.first_char;
      hash *= 1099511628211ULL;
    }
  }

  return hash;
}
