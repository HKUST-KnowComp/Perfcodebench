#include "interface.h"

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

void make_input(std::vector<float>& a, std::vector<float>& b, std::size_t n) {
  a.resize(n);
  b.resize(n);
  uint64_t state = 0x9999aaaabbbbccccULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    a[i] = static_cast<float>((state % 10000ULL) / 100.0 - 50.0);
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    b[i] = static_cast<float>((state % 10000ULL) / 100.0 - 50.0);
  }
}

double expected_dot(const std::vector<float>& a, const std::vector<float>& b) {
  double sum = 0.0;
  for (std::size_t i = 0; i < a.size(); ++i) {
    sum += static_cast<double>(a[i]) * static_cast<double>(b[i]);
  }
  return sum;
}

}  // namespace

int main() {
  std::vector<float> a;
  std::vector<float> b;
  make_input(a, b, 1 << 22);
  const double expected = expected_dot(a, b);

  const auto start = std::chrono::steady_clock::now();
  const float actual = dot_product(a.data(), b.data(), a.size());
  const auto end = std::chrono::steady_clock::now();

  const double diff = std::abs(static_cast<double>(actual) - expected);
  const double scale = std::max(1.0, std::abs(expected));
  const bool ok = diff / scale <= 1e-5;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_dot\":" << expected << ","
            << "\"actual_dot\":" << actual << "}\n";
  return ok ? 0 : 1;
}
