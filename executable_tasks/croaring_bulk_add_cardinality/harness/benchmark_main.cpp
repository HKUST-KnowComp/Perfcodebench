#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint32_t> make_values(std::size_t n) {
  std::vector<uint32_t> out;
  out.reserve(n);
  uint64_t state = 0x8421fedc3579ab60ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    uint32_t value = static_cast<uint32_t>((state >> 12) & 0x00FFFFFFU);
    if ((i % 7U) == 0U) {
      value &= ~0x3FU;
    }
    out.push_back(value);
  }
  std::sort(out.begin(), out.end());
  return out;
}

uint64_t expected_cardinality(const std::vector<uint32_t>& values) {
  std::vector<uint32_t> dedup = values;
  dedup.erase(std::unique(dedup.begin(), dedup.end()), dedup.end());
  return static_cast<uint64_t>(dedup.size());
}

}  // namespace

int main() {
  const std::vector<uint32_t> values = make_values(1200000);
  const uint64_t expected = expected_cardinality(values);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = bulk_add_cardinality(values);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_cardinality\":" << expected << ","
            << "\"actual_cardinality\":" << actual << "}\n";
  return ok ? 0 : 1;
}
