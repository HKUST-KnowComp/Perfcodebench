#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
  simdutf::implementation& impl = *simdutf::get_available_implementations().detect_best_supported();
  
  size_t utf16_length = impl.utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(utf16_length);
  
  size_t actual_length = impl.convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < actual_length; ++i) {
    hash ^= static_cast<uint64_t>(out[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}