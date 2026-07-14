#include "interface.h"
#include "xxhash.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  XXH32_state_t* state = XXH32_createState();
  
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    
    XXH32_reset(state, 0);
    XXH32_update(state, input.data() + offset, take);
    uint32_t chunk_hash = XXH32_digest(state);
    
    acc ^= static_cast<uint64_t>(chunk_hash);
    acc = (acc << 1) | (acc >> 63);
  }
  
  XXH32_freeState(state);
  return acc;
}