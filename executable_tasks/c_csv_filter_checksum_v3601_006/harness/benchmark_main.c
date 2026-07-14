#include "interface.h"

#include <inttypes.h>
#include <stdbool.h>
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
    int p1 = 0;
    while (row[p1] && row[p1] != ',') ++p1;
    int p2 = p1 + 1;
    while (row[p2] && row[p2] != ',') ++p2;
    int p3 = p2 + 1;
    while (row[p3] && row[p3] != ',') ++p3;
    int p4 = p3 + 1;
    while (row[p4] && row[p4] != ',') ++p4;
    int b = parse_int_at(row, p2 + 1);
    int c = parse_int_at(row, p3 + 1);
    bool hot = row[p4 + 1] == 'h';
    uint64_t v = (uint64_t)(b + c + 25);
    if (hot) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}

int main(void) {
  const size_t count = 317200u;
  char** rows = (char**)malloc(count * sizeof(char*));
  if (!rows) return 2;
  uint64_t state = 0x8877665544332211ULL;
  for (size_t i = 0; i < count; ++i) {
    rows[i] = (char*)malloc(64u);
    if (!rows[i]) return 3;
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    int a = (int)((state >> 16) & 65535ULL);
    int b = (int)((state >> 28) & 2047ULL);
    int c = (int)((state >> 40) & 1023ULL);
    const char* flag = ((i % 9u) == 0u) ? "hot" : "cold";
    snprintf(rows[i], 64u, "r%zu,%d,%d,%d,%s", i % 997u, a, b, c, flag);
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
