#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return checksum_bytes("");
  }

  // simdutf::convert_utf16to8_le expects a pointer to char16_t
  // We use a temporary string to hold the result.
  // simdutf provides highly optimized SIMD paths for this conversion.
  std::string out;
  try {
    // We use the simdutf API to convert the input. 
    // Note: simdutf::convert_utf16to8_le is designed for little-endian UTF-16.
    // Since std::u16string is platform-dependent but usually LE on x86, 
    // and the task specifies UTF-16LE, this is the correct high-performance path.
    
    // To avoid multiple allocations, we can estimate size, but simdutf handles 
    // the allocation/resizing internally or via a provided buffer. 
    // Here we use the simplest high-performance interface.
    
    size_t max_utf8_size = input.size() * 3; // Heuristic for reserve
    out.resize(max_utf8_size);
    
    size_t utf8_len = simdutf::convert_utf16to8_le(
        reinterpret_cast<const char16_t*>(input.data()), 
        input.size(), 
        reinterpret_cast<char*>(out.data())
    );
    
    out.resize(utf8_len);
  } catch (...) {
    // Fallback or error handling if necessary, though simdutf is robust for valid input
    return 0;
  }

  return checksum_bytes(out);
}
