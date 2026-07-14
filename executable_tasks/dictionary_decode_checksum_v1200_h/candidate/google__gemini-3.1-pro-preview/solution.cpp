#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

struct Entry {
  uint64_t size;
  uint64_t first_char;
};

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  // Pre-calculate dictionary entry properties to avoid string overhead and improve cache locality.
  // We only need the size and the first character of each string for the checksum.
  std::vector<Entry> prepped_dict;
  prepped_dict.reserve(dict.size());
  for (const auto& s : dict) {
    prepped_dict.push_back({static_cast<uint64_t>(s.size()), 
                            s.empty() ? 0ULL : static_cast<uint64_t>(static_cast<unsigned char>(s[0]))});
  }

  uint64_t hash = 0;
  const Entry* const dict_ptr = prepped_dict.data();
  const uint32_t* const codes_ptr = codes.data();
  const size_t codes_size = codes.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET_BASIS;
    
    // Unrolling the loop manually or relying on the compiler to optimize the access pattern.
    // Since the checksum is strictly sequential (FNV-1a), we process codes one by one.
    for (size_t i = 0; i < codes_size; ++i) {
      const Entry& entry = dict_ptr[codes_ptr[i]];
      
      // Inline the mix logic: hash = (hash ^ value) * prime
      hash ^= entry.size;
      hash *= FNV_PRIME;
      hash ^= entry.first_char;
      hash *= FNV_PRIME;
    }
  }

  return hash;
}
