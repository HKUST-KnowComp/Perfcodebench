#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>

#include <simdutf.h>

namespace {

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data.data());
  const unsigned char* end = ptr + data.size();
  while (ptr + 8 <= end) {
    uint64_t chunk;
    std::memcpy(&chunk, ptr, sizeof(chunk));
    hash ^= (chunk & 0xFF); hash *= 1099511628211ULL;
    hash ^= ((chunk >> 8) & 0xFF); hash *= 1099511628211ULL;
    hash ^= ((chunk >> 16) & 0xFF); hash *= 1099511628211ULL;
    hash ^= ((chunk >> 24) & 0xFF); hash *= 1099511628211ULL;
    hash ^= ((chunk >> 32) & 0xFF); hash *= 1099511628211ULL;
    hash ^= ((chunk >> 40) & 0xFF); hash *= 1099511628211ULL;
    hash ^= ((chunk >> 48) & 0xFF); hash *= 1099511628211ULL;
    hash ^= ((chunk >> 56) & 0xFF); hash *= 1099511628211ULL;
    ptr += 8;
  }
  while (ptr < end) {
    hash ^= *ptr;
    hash *= 1099511628211ULL;
    ++ptr;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  // Compute output length using simdutf (takes surrogate pairs into account)
  size_t utf8_len = simdutf::utf8_length_from_utf16le(input.data(), input.size());
  std::string out(utf8_len, '\0');
  // Fast valid conversion (input is guaranteed valid)
  size_t written = simdutf::convert_valid_utf16le_to_utf8(input.data(), input.size(), out.data());
  if (written != utf8_len) {
    out.resize(written);  // safety net
  }
  return checksum_bytes(out);
}