#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint32_t> make_input(int rows, int cols) {
  std::vector<uint32_t> out(static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols));
  uint64_t state = 0xabcdef0123456789ULL;
  for (std::size_t i = 0; i < out.size(); ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out[i] = static_cast<uint32_t>(state & 1023u);
  }
  return out;
}

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t expected_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);
  for (int step = 0; step < steps; ++step) {
    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        const std::size_t idx =
            static_cast<std::size_t>(row) * static_cast<std::size_t>(cols) + static_cast<std::size_t>(col);
        if (row == 0 || col == 0 || row == rows - 1 || col == cols - 1) {
          b[idx] = a[idx];
        } else {
          b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                    a[idx - static_cast<std::size_t>(cols)] +
                    a[idx + static_cast<std::size_t>(cols)]) /
                   8u;
        }
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}

}  // namespace

int main() {
  constexpr int kRows = 6144;
  constexpr int kCols = 512;
  constexpr int kSteps = 24;

  const std::vector<uint32_t> input = make_input(kRows, kCols);
  const uint64_t expected = expected_checksum(input, kRows, kCols, kSteps);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = jacobi_checksum(input, kRows, kCols, kSteps);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_checksum\":" << expected << ","
            << "\"actual_checksum\":" << actual << "}\n";
  return ok ? 0 : 1;
}
