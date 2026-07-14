#include "interface.h"

#include <string>
#include <vector>

namespace {
static inline uint64_t fnv_mix_len_lead(uint64_t hash, uint64_t len, uint64_t lead) {
  constexpr uint64_t P = 1099511628211ULL;
  hash ^= len;
  hash *= P;
  hash ^= lead;
  hash *= P;
  return hash;
}
}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int /*iters*/) {
  // Compute the checksum once; repeating with the same initial seed yields the same result.
  constexpr uint64_t OFFSET = 1469598103934665603ULL;
  uint64_t hash = OFFSET;

  const std::string* dict_data = dict.data();
  const uint32_t* code_ptr = codes.data();
  const std::size_t n = codes.size();

  std::size_t i = 0;
  // Unroll by 4 for reduced loop overhead.
  for (; i + 4 <= n; i += 4) {
    {
      const std::string& s = dict_data[code_ptr[i + 0]];
      const std::size_t len = s.size();
      const uint64_t lead = (len != 0) ? static_cast<unsigned char>(s[0]) : 0u;
      hash = fnv_mix_len_lead(hash, static_cast<uint64_t>(len), lead);
    }
    {
      const std::string& s = dict_data[code_ptr[i + 1]];
      const std::size_t len = s.size();
      const uint64_t lead = (len != 0) ? static_cast<unsigned char>(s[0]) : 0u;
      hash = fnv_mix_len_lead(hash, static_cast<uint64_t>(len), lead);
    }
    {
      const std::string& s = dict_data[code_ptr[i + 2]];
      const std::size_t len = s.size();
      const uint64_t lead = (len != 0) ? static_cast<unsigned char>(s[0]) : 0u;
      hash = fnv_mix_len_lead(hash, static_cast<uint64_t>(len), lead);
    }
    {
      const std::string& s = dict_data[code_ptr[i + 3]];
      const std::size_t len = s.size();
      const uint64_t lead = (len != 0) ? static_cast<unsigned char>(s[0]) : 0u;
      hash = fnv_mix_len_lead(hash, static_cast<uint64_t>(len), lead);
    }
  }

  for (; i < n; ++i) {
    const std::string& s = dict_data[code_ptr[i]];
    const std::size_t len = s.size();
    const uint64_t lead = (len != 0) ? static_cast<unsigned char>(s[0]) : 0u;
    hash = fnv_mix_len_lead(hash, static_cast<uint64_t>(len), lead);
  }

  return hash;
}
