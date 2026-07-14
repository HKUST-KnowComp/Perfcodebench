#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct Input {
  std::vector<uint32_t> row_ptr;
  std::vector<uint32_t> values;
};

Input make_input(std::size_t rows) {
  Input out;
  out.row_ptr.resize(rows + 1, 0);
  uint64_t state = 0x3141592653589793ULL;
  for (std::size_t r = 0; r < rows; ++r) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.row_ptr[r + 1] = out.row_ptr[r] + static_cast<uint32_t>((state & 15u) + 1u);
  }
  out.values.resize(static_cast<std::size_t>(out.row_ptr.back()));
  for (std::size_t i = 0; i < out.values.size(); ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.values[i] = static_cast<uint32_t>((state >> 16) & 1023u);
  }
  return out;
}

uint64_t expected_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t r = 0; r + 1 < row_ptr.size(); ++r) {
    uint64_t sum = 0;
    for (uint32_t i = row_ptr[r]; i < row_ptr[r + 1]; ++i) {
      sum += static_cast<uint64_t>(values[static_cast<std::size_t>(i)]);
    }
    hash = mix(hash, sum);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kRows = 1u << 18;
  constexpr int kIters = 8;

  const Input input = make_input(kRows);
  const uint64_t expected = expected_hash(input.row_ptr, input.values);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = csr_row_hash(input.row_ptr, input.values, kIters);
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
