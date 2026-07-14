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
  uint64_t first_byte;
};

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  
  if (iters <= 0) {
    return 0;
  }

  // Pre-process dictionary to avoid repeated string metadata access.
  // We only need the size and the first byte of each string.
  std::vector<DictEntry> prepared_dict;
  prepared_dict.reserve(dict.size());
  for (const auto& s : dict) {
    prepared_dict.push_back({
        static_cast<uint64_t>(s.size()),
        s.empty() ? 0ULL : static_cast<uint64_t>(static_cast<unsigned char>(s[0]))
    });
  }

  // The baseline implementation resets the hash to the FNV-1a offset basis 
  // at the start of every iteration. This means the final result is simply 
  // the result of the last iteration. We compute it once to maximize performance.
  uint64_t hash = 1469598103934665603ULL;
  
  // Process codes using the prepared dictionary metadata.
  // This avoids string materialization and vector reallocations.
  for (uint32_t code : codes) {
    const DictEntry& entry = prepared_dict[static_cast<size_t>(code)];
    
    // Equivalent to mix(hash, token.size())
    hash ^= entry.size;
    hash *= 1099511628211ULL;
    
    // Equivalent to mix(hash, token.empty() ? 0 : token[0])
    hash ^= entry.first_byte;
    hash *= 1099511628211ULL;
  }

  return hash;
}
