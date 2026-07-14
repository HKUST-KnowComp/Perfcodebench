#include "interface.h"

#include <fast_float/fast_float.h>

#include <string>

namespace {

inline bool parse_fortran_double(const char* begin, const char* end, double& out) {
  const char* p = begin;
  while (p < end && (*p == ' ' || *p == '\t' || *p == '\r')) {
    ++p;
  }
  if (p == end) {
    return false;
  }

  bool negative = false;
  if (*p == '+' || *p == '-') {
    negative = (*p == '-');
    ++p;
    if (p == end) {
      return false;
    }
  }

  const char* q = p;
  while (q < end) {
    const char c = *q;
    if (c == 'd' || c == 'D') {
      break;
    }
    ++q;
  }

  if (q == end) {
    double value;
    auto res = fast_float::from_chars(p, end, value);
    if (res.ec != std::errc() || res.ptr != end) {
      return false;
    }
    out = negative ? -value : value;
    return true;
  }

  double mantissa;
  auto res1 = fast_float::from_chars(p, q, mantissa);
  if (res1.ec != std::errc() || res1.ptr != q) {
    return false;
  }

  const char* e = q + 1;
  if (e == end) {
    return false;
  }

  bool exp_negative = false;
  if (*e == '+' || *e == '-') {
    exp_negative = (*e == '-');
    ++e;
    if (e == end) {
      return false;
    }
  }

  int exponent = 0;
  const char* exp_ptr = e;
  while (exp_ptr < end) {
    const unsigned char ch = static_cast<unsigned char>(*exp_ptr);
    if (ch < '0' || ch > '9') {
      return false;
    }
    exponent = exponent * 10 + static_cast<int>(ch - '0');
    ++exp_ptr;
  }

  if (exp_negative) {
    exponent = -exponent;
  }

  out = fast_float::integer_times_pow10(mantissa, exponent);
  if (negative) {
    out = -out;
  }
  return true;
}

}  // namespace

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    const char* line_end = p;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }

    double value;
    if (parse_fortran_double(p, line_end, value)) {
      sum += value;
    } else {
      break;
    }

    p = (line_end < end) ? (line_end + 1) : line_end;
  }

  return sum;
}
