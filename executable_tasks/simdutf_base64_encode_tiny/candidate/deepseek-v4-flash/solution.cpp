#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace {
constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr uint64_t kFnvPrime = 1099511628211ULL;
}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const std::size_t in_len = input.size();
  const std::size_t out_len = ((in_len + 2) / 3) * 4;

  // Use a stack buffer for tiny payloads (up to 384 input bytes -> 512 output bytes)
  char buf[512];
  uint64_t hash = kFnvOffset;

  if (out_len <= sizeof(buf)) {
    char* p = buf;
    for (std::size_t i = 0; i < in_len; i += 3) {
      const uint32_t b0 = static_cast<unsigned char>(input[i]);
      const uint32_t b1 =
          (i + 1 < in_len) ? static_cast<unsigned char>(input[i + 1]) : 0U;
      const uint32_t b2 =
          (i + 2 < in_len) ? static_cast<unsigned char>(input[i + 2]) : 0U;
      const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

      char c;
      c = kTable[(triple >> 18) & 0x3FU];
      hash ^= static_cast<unsigned char>(c);
      hash *= kFnvPrime;
      *p++ = c;

      c = kTable[(triple >> 12) & 0x3FU];
      hash ^= static_cast<unsigned char>(c);
      hash *= kFnvPrime;
      *p++ = c;

      if (i + 1 < in_len) {
        c = kTable[(triple >> 6) & 0x3FU];
      } else {
        c = '=';
      }
      hash ^= static_cast<unsigned char>(c);
      hash *= kFnvPrime;
      *p++ = c;

      if (i + 2 < in_len) {
        c = kTable[triple & 0x3FU];
      } else {
        c = '=';
      }
      hash ^= static_cast<unsigned char>(c);
      hash *= kFnvPrime;
      *p++ = c;
    }
  } else {
    // Fallback for larger inputs (should not occur in tiny-payload use)
    std::string out;
    out.reserve(out_len);
    for (std::size_t i = 0; i < in_len; i += 3) {
      const uint32_t b0 = static_cast<unsigned char>(input[i]);
      const uint32_t b1 =
          (i + 1 < in_len) ? static_cast<unsigned char>(input[i + 1]) : 0U;
      const uint32_t b2 =
          (i + 2 < in_len) ? static_cast<unsigned char>(input[i + 2]) : 0U;
      const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
      out.push_back(kTable[(triple >> 18) & 0x3FU]);
      out.push_back(kTable[(triple >> 12) & 0x3FU]);
      out.push_back((i + 1 < in_len) ? kTable[(triple >> 6) & 0x3FU] : '=');
      out.push_back((i + 2 < in_len) ? kTable[triple & 0x3FU] : '=');
    }
    // Compute checksum on the heap-allocated string
    for (unsigned char ch : out) {
      hash ^= static_cast<uint64_t>(ch);
      hash *= kFnvPrime;
    }
  }

  return hash;
}
