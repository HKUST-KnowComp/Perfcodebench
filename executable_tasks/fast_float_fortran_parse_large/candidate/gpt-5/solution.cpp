#include "interface.h"

#include <string>
#include <cstdlib>
#include <cstring>
#include <fast_float/fast_float.h>

static inline const char* find_newline(const char* p, const char* end) {
  const void* nl = std::memchr(p, '\n', static_cast<size_t>(end - p));
  return nl ? static_cast<const char*>(nl) : end;
}

static inline void trim_spaces(const char*& s, const char*& e) {
  // Trim trailing CR first (handle CRLF)
  if (e > s && e[-1] == '\r') {
    --e;
  }
  while (s < e && (*s == ' ' || *s == '\t')) ++s;
  while (e > s && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\r')) --e;
}

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  std::string tmp; // reusable buffer for D/d replacement when necessary

  while (p < end) {
    const char* line_end = find_newline(p, end);

    const char* s = p;
    const char* e = line_end;
    trim_spaces(s, e);

    if (s < e) {
      // Detect presence of Fortran exponent (D/d)
      bool hasD = false;
      for (const char* q = s; q < e; ++q) {
        char c = *q;
        if (c == 'D' || c == 'd') { hasD = true; break; }
      }

      if (!hasD) {
        double v;
        auto r = fast_float::from_chars(s, e, v);
        if (r.ec == std::errc()) {
          sum += v;
        } else {
          // Fallback on rare parse failures
          std::string t(s, e);
          char* next = nullptr;
          double v2 = std::strtod(t.c_str(), &next);
          if (next != t.c_str()) sum += v2;
        }
      } else {
        // Replace D/d with E/e in a reusable buffer
        tmp.assign(s, e);
        for (char& ch : tmp) {
          if (ch == 'd') ch = 'e';
          else if (ch == 'D') ch = 'E';
        }
        double v;
        auto r = fast_float::from_chars(tmp.data(), tmp.data() + tmp.size(), v);
        if (r.ec == std::errc()) {
          sum += v;
        } else {
          // Fallback if needed
          char* next = nullptr;
          double v2 = std::strtod(tmp.c_str(), &next);
          if (next != tmp.c_str()) sum += v2;
        }
      }
    }

    p = (line_end < end) ? line_end + 1 : line_end;
  }

  return sum;
}
