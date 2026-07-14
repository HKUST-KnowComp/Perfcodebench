#include "interface.h"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {

std::string make_input(std::size_t count) {
  std::ostringstream out;
  out << std::setprecision(17);
  uint64_t state = 0x1337c0ffeeULL;
  for (std::size_t i = 0; i < count; ++i) {
    state = state * 6364136223846793005ULL + 1ULL;
    const double integer_part = static_cast<double>((state >> 20) % 100000);
    const double frac_part = static_cast<double>(state % 1000000) / 1000000.0;
    const double sign = (state & 1ULL) ? -1.0 : 1.0;
    if (i != 0) {
      out << ';';
    }
    out << sign * (integer_part + frac_part);
  }
  return out.str();
}

double expected_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    char* next = nullptr;
    const double value = std::strtod(p, &next);
    if (next == p) {
      break;
    }
    sum += value;
    p = (*next == ';') ? next + 1 : next;
  }
  return sum;
}

}  // namespace

int main() {
  const std::string input = make_input(200000);
  const double expected = expected_sum(input);

  const auto start = std::chrono::steady_clock::now();
  const double actual = parse_sum(input);
  const auto end = std::chrono::steady_clock::now();

  const double diff = std::abs(actual - expected);
  const double scale = std::max(1.0, std::abs(expected));
  const bool ok = diff / scale <= 1e-12;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_sum\":" << std::setprecision(17) << expected << ","
            << "\"actual_sum\":" << std::setprecision(17) << actual << "}\n";
  return ok ? 0 : 1;
}
