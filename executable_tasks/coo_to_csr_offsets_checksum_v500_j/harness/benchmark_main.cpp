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

std::vector<uint32_t> make_input(std::size_t rows) {
  std::vector<uint32_t> row_ids;
  row_ids.reserve(rows * 8u);
  uint64_t state = 0xabcdef0123456789ULL;
  for (std::size_t r = 0; r < rows; ++r) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    const uint32_t len = static_cast<uint32_t>((state & 15u) + 1u);
    for (uint32_t i = 0; i < len; ++i) {
      row_ids.push_back(static_cast<uint32_t>(r));
    }
  }
  return row_ids;
}

uint64_t expected_hash(const std::vector<uint32_t>& row_ids, uint32_t rows) {
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u, 0);
  for (uint32_t row : row_ids) {
    ++offsets[static_cast<std::size_t>(row + 1u)];
  }
  for (uint32_t r = 0; r < rows; ++r) {
    offsets[static_cast<std::size_t>(r + 1u)] += offsets[static_cast<std::size_t>(r)];
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : offsets) {
    hash = mix(hash, v);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr uint32_t kRows = 1u << 18;
  constexpr int kIters = 6;

  const std::vector<uint32_t> row_ids = make_input(kRows);
  const uint64_t expected = expected_hash(row_ids, kRows);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = csr_offsets_hash(row_ids, kRows, kIters);
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
