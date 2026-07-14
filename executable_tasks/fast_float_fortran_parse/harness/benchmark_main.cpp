#include "interface.h"

#include <algorithm>
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
  uint64_t state = 0x3141592653589793ULL;
  for (std::size_t i = 0; i < count; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    const double mantissa = static_cast<double>((state >> 12) % 1000000ULL) / 100000.0;
    const int exponent = static_cast<int>((state % 23ULL)) - 11;
    const double sign = (state & 1ULL) ? -1.0 : 1.0;
    if (i != 0) {
      out << '\n';
    }
    out << std::showpos << sign * mantissa << 'd' << exponent << std::noshowpos;
  }
  return out.str();
}

double expected_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    const char* line_end = p;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    std::string token(p, line_end);
    std::replace(token.begin(), token.end(), 'd', 'e');
    std::replace(token.begin(), token.end(), 'D', 'E');
    char* next = nullptr;
    const double value = std::strtod(token.c_str(), &next);
    if (next == token.c_str()) {
      break;
    }
    sum += value;
    p = (line_end < end) ? line_end + 1 : line_end;
  }
  return sum;
}

}  // namespace

int main() {
  const std::string input = make_input(180000);
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
