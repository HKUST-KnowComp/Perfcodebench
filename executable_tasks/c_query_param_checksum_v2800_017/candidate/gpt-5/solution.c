#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int fast_parse_int(const char* p) {
  // Skip leading whitespace (like atoi)
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '\v' || *p == '\f') {
    ++p;
  }
  int sign = 1;
  if (*p == '+') {
    ++p;
  } else if (*p == '-') {
    sign = -1;
    ++p;
  }
  unsigned int val = 0;
  unsigned char c;
  while ((c = (unsigned char)*p) >= '0' && c <= '9') {
    val = val * 10u + (unsigned int)(c - '0');
    ++p;
  }
  return sign < 0 ? -(int)val : (int)val;
}

uint64_t run(const char* const* rows, size_t row_count) {
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t hash = FNV_OFFSET_BASIS;

  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    const char* amp1 = NULL;
    const char* amp2 = NULL;

    // Find first two '&' quickly in a single pass; stop after second is found
    const char* p = s;
    for (;; ++p) {
      char ch = *p;
      if (ch == '\0') break;
      if (ch == '&') {
        if (!amp1) {
          amp1 = p;
        } else {
          amp2 = p;
          break; // we only need first two
        }
      }
    }

    int u = 0, sv = 0, f = 0;

    // Parse u from first token at offset +2
    if (s && s[0] != '\0' && s[1] != '\0') {
      u = fast_parse_int(s + 2);
    }

    // Parse s from second token at offset +6
    if (amp1) {
      const char* t1 = amp1 + 1;
      // Ensure we don't read before string end; if shorter than 6, fast_parse_int will just handle whatever is there
      sv = fast_parse_int(t1 + 6);
    }

    // Parse f from third token at offset +5
    if (amp2) {
      const char* t2 = amp2 + 1;
      f = fast_parse_int(t2 + 5);
    }

    uint64_t v = (uint64_t)((u & 2047) + sv + f * 19);
    hash ^= (v + 1ULL);
    hash *= FNV_PRIME;
  }

  return hash;
}
