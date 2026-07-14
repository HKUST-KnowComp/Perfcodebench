#include "interface.h"

#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const char* ptr = input.data();
  const std::size_t len = input.size();
  const std::size_t num_full_chunks = len / kChunkSize;
  const std::size_t remainder = len - num_full_chunks * kChunkSize;
  
  uint64_t acc = 0;
  
  // Process 4 chunks at a time for better ILP
  const std::size_t unroll = 4;
  const std::size_t full_iters = num_full_chunks / unroll;
  const std::size_t leftover_chunks = num_full_chunks % unroll;
  
  for (std::size_t i = 0; i < full_iters; ++i) {
    // Compute 4 hashes in parallel
    uint32_t h0 = XXH32(ptr, kChunkSize, 0);
    uint32_t h1 = XXH32(ptr + kChunkSize, kChunkSize, 0);
    uint32_t h2 = XXH32(ptr + 2 * kChunkSize, kChunkSize, 0);
    uint32_t h3 = XXH32(ptr + 3 * kChunkSize, kChunkSize, 0);
    ptr += 4 * kChunkSize;
    
    // Combine in order: acc ^= h0; rotate; acc ^= h1; rotate; ...
    acc ^= static_cast<uint64_t>(h0);
    acc = (acc << 1) | (acc >> 63);
    acc ^= static_cast<uint64_t>(h1);
    acc = (acc << 1) | (acc >> 63);
    acc ^= static_cast<uint64_t>(h2);
    acc = (acc << 1) | (acc >> 63);
    acc ^= static_cast<uint64_t>(h3);
    acc = (acc << 1) | (acc >> 63);
  }
  
  // Handle remaining full chunks
  for (std::size_t i = 0; i < leftover_chunks; ++i) {
    acc ^= static_cast<uint64_t>(XXH32(ptr, kChunkSize, 0));
    acc = (acc << 1) | (acc >> 63);
    ptr += kChunkSize;
  }
  
  // Handle final partial chunk if any
  if (remainder > 0) {
    acc ^= static_cast<uint64_t>(XXH32(ptr, remainder, 0));
    acc = (acc << 1) | (acc >> 63);
  }
  
  return acc;
}