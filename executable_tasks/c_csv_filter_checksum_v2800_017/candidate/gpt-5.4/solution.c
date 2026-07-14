#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline const char* skip_to_next_comma(const char* p) {
  while (*p != ',' && *p != '\0') ++p;
  if (*p == ',') ++p;
  return p;
}

static inline int parse_int_advance(const char** pp) {
  const char* p = *pp;
  int sign = 1;
  int v = 0;
  if (*p == '-') {
    sign = -1;
    ++p;
  }
  while ((unsigned)(*p - '0') < 10u) {
    v = v * 10 + (int)(*p - '0');
    ++p;
  }
  if (*p == ',') ++p;
  *pp = p;
  return v * sign;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    p = skip_to_next_comma(p);      /* skip r{id} */
    (void)parse_int_advance(&p);    /* skip a */
    int b = parse_int_advance(&p);  /* parse b */
    int c = parse_int_advance(&p);  /* parse c */

    uint64_t v = (uint64_t)(b + c + 24);

    /* flag is the final field; baseline uses %7s and strcmp("hot").
       Since the field is last in the CSV row, exact match to "hot"
       means the remaining bytes are 'h','o','t','\0'. */
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
      v += 29ULL;
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
