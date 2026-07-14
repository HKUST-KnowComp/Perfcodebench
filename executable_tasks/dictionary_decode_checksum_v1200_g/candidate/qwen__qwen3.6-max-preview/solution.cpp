#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  if (iters <= 0) return 0;

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  const std::string* dict_data = dict.data();

  for (uint32_t code : codes) {
    const std::string& token = dict_data[static_cast<std::size_t>(code)];
    std::size_t len = token.size();
    
    hash ^= len;
    hash *= prime;
    
    hash ^= (len == 0 ? 0u : static_cast<unsigned char>(token[0]));
    hash *= prime;
  }
  
  return hash;
}
