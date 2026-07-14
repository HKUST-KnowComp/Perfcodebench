#include "interface.h"
#include <simdutf.h>
#include <cstring>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  
  // Process 8 bytes at a time
  for (; i + 7 < len; i += 8) {
    uint64_t chunk;
    std::memcpy(&chunk, data + i, 8);
    
    // Unroll the loop for 8 bytes
    hash ^= (chunk & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 8) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 16) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 24) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 32) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 40) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 48) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 56) & 0xFF);
    hash *= prime;
  }
  
  // Handle remaining bytes
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= prime;
  }
  
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  // Use simdutf for optimized Latin-1 to UTF-8 conversion
  size_t expected_len = simdutf::utf8_length_from_latin1(input.data(), input.size());
  std::string out(expected_len, '\0');
  
  size_t actual_len = simdutf::convert_latin1_to_utf8(input.data(), input.size(), out.data());
  out.resize(actual_len);
  
  return checksum_bytes(out.data(), out.size());
}
