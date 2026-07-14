#include "interface.h"

#include "simdutf.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

static inline uint64_t checksum_utf16(const char16_t* data, size_t size) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL; // FNV-1a 64-bit prime
  }
  return hash;
}

uint64_t transcode_checksum(const std::string& input) {
  const char* in = input.data();
  const size_t in_len = input.size();

  // Compute required UTF-16 code units and allocate exactly once
  const size_t out_len = simdutf::utf16_length_from_utf8(in, in_len);
  std::vector<char16_t> out(out_len);

  // Convert UTF-8 -> UTF-16LE using SIMD-accelerated routine
  // Inputs are guaranteed valid per task, but this function also validates.
  const size_t written = simdutf::convert_utf8_to_utf16le(in, in_len, out.data());

  // written should match out_len for valid inputs
  return checksum_utf16(out.data(), written);
}
