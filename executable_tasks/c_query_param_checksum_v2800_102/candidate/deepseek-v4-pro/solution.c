#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    /* parse u: skip 2 bytes, then digits until '&' */
    const char* p = row + 2;
    int u = 0;
    while (*p >= '0' && *p <= '9') {
      u = u * 10 + (*p - '0');
      ++p;
    }
    /* p now points to '&' separating field0 and field1 */
    const char* q = p + 1;  /* start of field1 */
    /* parse s: skip 6 bytes from q, then digits until next '&' */
    const char* s_digits = q + 6;
    int s = 0;
    while (*s_digits >= '0' && *s_digits <= '9') {
      s = s * 10 + (*s_digits - '0');
      ++s_digits;
    }
    /* s_digits now points to '&' separating field1 and field2 */
    const char* r = s_digits + 1;  /* start of field2 */
    /* parse f: skip 5 bytes from r, then digits until end of string */
    const char* f_digits = r + 5;
    int f = 0;
    while (*f_digits >= '0' && *f_digits <= '9') {
      f = f * 10 + (*f_digits - '0');
      ++f_digits;
    }
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}