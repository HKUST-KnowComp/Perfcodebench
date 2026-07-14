#include "interface.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int parse_int_at(const char* s, int start) {
  int v = 0;
  while (s[start] >= '0' && s[start] <= '9') {
    v = v * 10 + (s[start] - '0');
    ++start;
  }
  return v;
}

static uint64_t expected_checksum(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    int up = 2;
    int sp = 0;
    while (row[sp] && !(row[sp] == '&' && row[sp + 1] == 's')) ++sp;
    sp += 7;
    int fp = 0;
    while (row[fp] && !(row[fp] == '&' && row[fp + 1] == 'f')) ++fp;
    fp += 6;
    int u = parse_int_at(row, up);
    int s = parse_int_at(row, sp);
    int f = parse_int_at(row, fp);
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}

int main(void) {
  const size_t count = 521700u;
  char** rows = (char**)malloc(count * sizeof(char*));
  if (!rows) return 2;
  uint64_t state = 0x88779966AABBCCDDULL;
  for (size_t i = 0; i < count; ++i) {
    rows[i] = (char*)malloc(64u);
    if (!rows[i]) return 3;
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    int u = (int)((state >> 16) & 65535ULL);
    int s = (int)((state >> 28) & 2047ULL);
    int f = ((i % 2u) == 0u) ? 1 : 0;
    snprintf(rows[i], 64u, "u=%d&score=%d&flag=%d&k=z%zu", u, s, f, i % 113u);
  }

  uint64_t expected = expected_checksum((const char* const*)rows, count);
  clock_t start = clock();
  uint64_t actual = run((const char* const*)rows, count);
  clock_t end = clock();
  long long elapsed_ns = (long long)(((end - start) * 1000000000.0) / CLOCKS_PER_SEC);
  int ok = (actual == expected);
  printf("{\"ok\":%s,\"elapsed_ns\":%lld,\"expected_checksum\":%" PRIu64 ",\"actual_checksum\":%" PRIu64 "}\n",
         ok ? "true" : "false",
         elapsed_ns,
         expected,
         actual);
  for (size_t i = 0; i < count; ++i) free(rows[i]);
  free(rows);
  return ok ? 0 : 1;
}
