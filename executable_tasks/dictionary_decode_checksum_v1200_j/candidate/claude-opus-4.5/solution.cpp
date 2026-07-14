#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  // Precompute length and leading byte for each dictionary entry
  const std::size_t dict_size = dict.size();
  std::vector<uint64_t> lengths(dict_size);
  std::vector<uint64_t> lead_bytes(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    lengths[i] = dict[i].size();
    lead_bytes[i] = dict[i].empty() ? 0u : static_cast<unsigned char>(dict[i][0]);
  }

  const uint64_t* __restrict len_ptr = lengths.data();
  const uint64_t* __restrict lead_ptr = lead_bytes.data();
  const uint32_t* __restrict code_ptr = codes.data();
  const std::size_t n = codes.size();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      const std::size_t idx = code_ptr[i];
      // mix length
      hash ^= len_ptr[idx];
      hash *= 1099511628211ULL;
      // mix leading byte
      hash ^= lead_ptr[idx];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
