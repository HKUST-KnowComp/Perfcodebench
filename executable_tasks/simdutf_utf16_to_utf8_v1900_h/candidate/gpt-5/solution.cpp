#include "interface.h"

#include <string>
#include "simdutf.h"

namespace {

inline uint64_t fnv1a_checksum(const char* data, size_t len) {
  constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t hash = FNV_OFFSET_BASIS;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
    hash *= FNV_PRIME;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  // Compute required UTF-8 length and allocate exact buffer once
  const char16_t* in_ptr = input.data();
  const size_t in_len = input.size();
  const size_t out_len = simdutf::utf8_length_from_utf16(in_ptr, in_len);

  std::string out;
  out.resize(out_len);

  // Perform SIMD-accelerated transcoding
  size_t written = simdutf::utf16_to_utf8(in_ptr, in_len, out.data());
  // Input is guaranteed valid by the task; still guard against unexpected mismatch
  if (written != out_len) {
    // In case of any discrepancy, adjust to actual written size
    out.resize(written);
  }

  // Compute FNV-1a checksum over exact UTF-8 bytes
  return fnv1a_checksum(out.data(), out.size());
}
