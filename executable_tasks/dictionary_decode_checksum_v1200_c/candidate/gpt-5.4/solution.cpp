#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct DictInfo {
  uint64_t len;
  uint64_t first;
};

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  std::vector<DictInfo> info;
  info.reserve(dict.size());
  for (const std::string& s : dict) {
    info.push_back(DictInfo{
        static_cast<uint64_t>(s.size()),
        static_cast<uint64_t>(s.empty() ? 0u : static_cast<unsigned char>(s[0]))});
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    const uint32_t* code_ptr = codes.data();
    const uint32_t* code_end = code_ptr + codes.size();

    for (; code_ptr != code_end; ++code_ptr) {
      const DictInfo& d = info[static_cast<std::size_t>(*code_ptr)];
      hash = mix(hash, d.len);
      hash = mix(hash, d.first);
    }
  }
  return hash;
}
