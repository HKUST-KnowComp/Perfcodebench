#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> boundaries;
  std::vector<uint32_t> probes;
};

Input make_input(std::size_t buckets, std::size_t rows) {
  Input out;
  out.boundaries.resize(buckets);
  uint32_t x = 0;
  for (std::size_t i = 0; i < buckets; ++i) {
    x += static_cast<uint32_t>((i & 3u) + 1u);
    out.boundaries[i] = x;
  }
  out.probes.resize(rows);
  uint64_t state = 0xfeedfacecafebeefULL;
  for (std::size_t i = 0; i < rows; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.probes[i] = static_cast<uint32_t>(state % static_cast<uint64_t>(x));
  }
  std::sort(out.probes.begin(), out.probes.end());
  return out;
}

uint64_t expected_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes) {
  uint64_t sum = 0;
  for (uint32_t probe : probes) {
    sum += static_cast<uint64_t>(
        std::upper_bound(boundaries.begin(), boundaries.end(), probe) - boundaries.begin());
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kBuckets = 4096;
  constexpr std::size_t kRows = 1u << 20;
  constexpr int kIters = 16;

  const Input input = make_input(kBuckets, kRows);
  const uint64_t expected = expected_sum(input.boundaries, input.probes);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = range_bucket_sum(input.boundaries, input.probes, kIters);
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
