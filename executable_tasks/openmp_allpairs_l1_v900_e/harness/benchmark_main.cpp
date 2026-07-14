#include "interface.h"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace {

void make_input(std::vector<int32_t>& xs, std::vector<int32_t>& ys, int n) {
  xs.resize(static_cast<std::size_t>(n));
  ys.resize(static_cast<std::size_t>(n));
  uint64_t state = 0x13579bdf2468ace0ULL;
  for (int i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    xs[static_cast<std::size_t>(i)] = static_cast<int32_t>((state % 2048ULL) - 1024);
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    ys[static_cast<std::size_t>(i)] = static_cast<int32_t>((state % 2048ULL) - 1024);
  }
}

int64_t expected_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys) {
  int64_t total = 0;
  for (std::size_t i = 0; i < xs.size(); ++i) {
    for (std::size_t j = i + 1; j < xs.size(); ++j) {
      total += static_cast<int64_t>(std::abs(xs[i] - xs[j])) +
               static_cast<int64_t>(std::abs(ys[i] - ys[j]));
    }
  }
  return total;
}

}  // namespace

int main() {
  constexpr int kCount = 1536;
  constexpr int kIters = 24;

  std::vector<int32_t> xs;
  std::vector<int32_t> ys;
  make_input(xs, ys, kCount);
  const int64_t expected = expected_sum(xs, ys);

  const auto start = std::chrono::steady_clock::now();
  const int64_t actual = allpairs_l1_sum(xs, ys, kIters);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
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
