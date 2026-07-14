#include "interface.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static bool is_hex(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

static uint64_t expected_checksum(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    int ok = s[0] >= 'A' && s[0] <= 'Z' &&
             s[1] >= 'A' && s[1] <= 'Z' &&
             s[2] >= '0' && s[2] <= '9' &&
             s[3] >= '0' && s[3] <= '9' &&
             s[4] >= '0' && s[4] <= '9' &&
             s[5] >= '0' && s[5] <= '9' &&
             s[6] == '-' &&
             is_hex(s[7]) && is_hex(s[8]) && is_hex(s[9]) && is_hex(s[10]) &&
             is_hex(s[11]) && is_hex(s[12]) && is_hex(s[13]) && is_hex(s[14]) &&
             s[15] == 0;
    uint64_t v = ok ? (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]) : 0ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}

int main(void) {
  const size_t count = 386600u;
  char** rows = (char**)malloc(count * sizeof(char*));
  if (!rows) return 2;
  uint64_t state = 0x1234432155667788ULL;
  for (size_t i = 0; i < count; ++i) {
    rows[i] = (char*)malloc(32u);
    if (!rows[i]) return 3;
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    char a = (char)('A' + ((state >> 60) % 26));
    char b = (char)('A' + ((state >> 56) % 26));
    int x = (int)((state >> 16) % 10000ULL);
    if ((i % 9u) == 0u) {
      snprintf(rows[i], 32u, "%c%c%04d-%08X", a, b, x, (unsigned int)state);
    } else {
      snprintf(rows[i], 32u, "bad:%08X", (unsigned int)state);
    }
  }
  uint64_t expected = expected_checksum((const char* const*)rows, count);
  clock_t start = clock();
  uint64_t actual = run((const char* const*)rows, count);
  clock_t end = clock();
  long long elapsed_ns = (long long)(((end - start) * 1000000000.0) / CLOCKS_PER_SEC);
  int ok = (actual == expected);
  printf("{\"ok\":%s,\"elapsed_ns\":%lld,\"expected_checksum\":%" PRIu64 ",\"actual_checksum\":%" PRIu64 "}\n",
         ok ? "true" : "false", elapsed_ns, expected, actual);
  for (size_t i = 0; i < count; ++i) free(rows[i]);
  free(rows);
  return ok ? 0 : 1;
}
