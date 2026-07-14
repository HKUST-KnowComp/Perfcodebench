#include "interface.h"
#include "simdutf.h"
#include <vector>

namespace {

inline uint64_t compute_checksum(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  simdutf::implementation* impl = simdutf::get_available_implementations().detect_best_supported();
  
  size_t max_len = impl->utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(max_len);
  
  size_t actual_len = impl->convert_utf8_to_utf16(input.data(), input.size(), out.data());
  
  return compute_checksum(out.data(), actual_len);
}