#include "interface.h"
#include "simdutf.h"
#include <string"
#include <vector>

namespace {

// FNV-1a hash implementation as per the baseline
uint64_t checksum_bytes(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
  if (input.empty()) {
    return checksum_bytes(nullptr, 0);
  }

  // simdutf::base64::unpack_padded handles standard base64 with padding.
  // We use a vector to manage the decoded buffer memory.
  // The size of the decoded buffer is at most 3/4 of the input size.
  std::vector<uint8_t> decoded_buffer;
  try {
    // simdutf::base64::unpack_padded returns a std::vector<uint8_t>
    // or we can use the version that writes to a buffer. 
    // To minimize allocations and use the library efficiently:
    size_t max_out_size = (input.size() / 4) * 3;
    decoded_buffer.resize(max_out_size);
    
    size_t decoded_size = simdutf::base64::unpack_padded(input.data(), input.size(), decoded_buffer.data());
    
    // The checksum must be calculated over the exact decoded bytes.
    return checksum_bytes(decoded_buffer.data(), decoded_size);
  } catch (...) {
    // Fallback or error handling if input is malformed, though task implies valid base64
    return 0;
  }
}
