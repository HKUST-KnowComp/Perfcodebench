#include "interface.h"

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

void make_input(std::vector<float>& data, std::size_t n) {
  data.resize(n);
  uint64_t state = 0x2718281828459045ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    data[i] = static_cast<float>((state % 40001ULL) / 100.0 - 200.0);
  }
}

double expected_sum(const std::vector<float>& data) {
  double sum = 0.0;
  for (float v : data) {
    sum += static_cast<double>(v > 0.0f ? v : 0.0f);
  }
  return sum;
}

}  // namespace

int main() {
  std::vector<float> data;
  make_input(data, 1 << 21);
  const double expected = expected_sum(data);

  const auto start = std::chrono::steady_clock::now();
  const float actual = clamp_sum(data.data(), data.size());
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
            << "\"expected_sum\":" << expected << ","
            << "\"actual_sum\":" << actual << "}\n";
  return ok ? 0 : 1;
}
