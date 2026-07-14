#include "interface.h"
#include <cstddef>
#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::string &input) {
  /* FNV-1a constants */
  const uint64_t offset_basis = 14695981039346656037ULL;
  const uint64_t prime = 1099511628211ULL;

  /* Streaming state */
  const unsigned char *p = reinterpret_cast<const unsigned char *>(input.data());
  const unsigned char *end = p + input.size();
  uint64_t hash = offset_basis;

  /* one-byte encoding: 0xxxxxxx
   * two-byte encoding: 110xxxxx 10xxxxxx
   * three-byte encoding: 1110xxxx 10xxxxxx 10xxxxxx
   * four-byte encoding: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   */
  while (p < end) {
    unsigned char c = *p++;
    uint32_t cp;

    if (c <= 0x7F) { /* 00..7F */
      cp = static_cast<uint32_t>(c);
    } else if ((c & 0xE0) == 0xC0) { /* 110xxxxx */
      if (p == end) break; /* malformed -> truncate; assume well-formed */
      cp = ((static_cast<uint32_t>(c & 0x1F)) << 6) |
           (static_cast<uint32_t>(*p & 0x3F));
      ++p;
    } else if ((c & 0xF0) == 0xE0) { /* 1110xxxx */
      if (p + 1 > end) break; /* malformed -> truncate; assume well-formed */
      cp = ((static_cast<uint32_t>(c & 0x0F)) << 12) |
           ((static_cast<uint32_t>(p[0] & 0x3F)) << 6) |
           (static_cast<uint32_t>(p[1] & 0x3F));
      p += 2;
    } else { /* assume (c & 0xF8) == 0xF0 → 11110xxx */
      if (p + 2 > end) break; /* malformed -> truncate; assume well-formed */
      cp = ((static_cast<uint32_t>(c & 0x07)) << 18) |
           ((static_cast<uint32_t>(p[0] & 0x3F)) << 12) |
           ((static_cast<uint32_t>(p[1] & 0x3F)) << 6) |
           (static_cast<uint32_t>(p[2] & 0x3F));
      p += 3;
    }

    /* Encode cp to UTF-16LE code unit(s) and update hash */
    if (cp <= 0xFFFF) {
      /* BMP character – one 16-bit unit */
      hash ^= static_cast<uint64_t>(static_cast<char16_t>(cp));
      hash *= prime;
    } else {
      /* Supplementary character – surrogate pair */
      cp -= 0x10000;
      char16_t lo = static_cast<char16_t>(0xDC00U + (cp & 0x3FFU));
      char16_t hi = static_cast<char16_t>(0xD800U + (cp >> 10));

      hash ^= static_cast<uint64_t>(hi);
      hash *= prime;
      hash ^= static_cast<uint64_t>(lo);
      hash *= prime;
    }
  }

  return hash;
}