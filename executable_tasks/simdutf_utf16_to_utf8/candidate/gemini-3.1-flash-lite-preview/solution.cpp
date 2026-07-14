#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t compute_fnv1a(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  simdutf::implementation& impl = *simdutf::get_available_implementations().front();
  
  size_t max_len = impl.utf8_length_from_utf16(input.size(), reinterpret_cast<const char16_t*>(input.data()));
  std::vector<char> buffer(max_len);
  
  size_t actual_len = impl.convert_utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size(), buffer.data());
  
  return compute_fnv1a(buffer.data(), actual_len);
}