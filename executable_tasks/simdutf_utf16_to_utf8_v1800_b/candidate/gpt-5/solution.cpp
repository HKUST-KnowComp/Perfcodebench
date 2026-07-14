#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <cstddef>

uint64_t transcode_checksum(const std::u16string& input) {
  // FNV-1a 64-bit offset basis
  uint64_t hash = 1469598103934665603ULL;
  const size_t in_size = input.size();
  if (in_size == 0) {
    return hash;
  }

  // Worst-case UTF-8 size from UTF-16 code units is 3 bytes per code unit
  const size_t out_capacity = in_size * 3;
  char* outbuf = new char[out_capacity];

  const size_t written = simdutf::convert_valid_utf16_to_utf8(input.data(), in_size, outbuf);

  const unsigned char* p = reinterpret_cast<const unsigned char*>(outbuf);
  const unsigned char* const end = p + written;
  while (p != end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL; // FNV prime
  }

  delete[] outbuf;
  return hash;
}
