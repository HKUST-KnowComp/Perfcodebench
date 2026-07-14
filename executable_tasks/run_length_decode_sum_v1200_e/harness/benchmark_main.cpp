#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> values;
  std::vector<uint16_t> lengths;
};

Input make_input(std::size_t runs) {
  Input out;
  out.values.resize(runs);
  out.lengths.resize(runs);
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  for (std::size_t i = 0; i < runs; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.values[i] = static_cast<uint32_t>((state >> 16) & 4095u);
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.lengths[i] = static_cast<uint16_t>((state & 31u) + 1u);
  }
  return out;
}

uint64_t expected_sum(const std::vector<uint32_t>& values, const std::vector<uint16_t>& lengths) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < values.size(); ++i) {
    sum += static_cast<uint64_t>(values[i]) * static_cast<uint64_t>(lengths[i]);
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kRuns = 786432u;
  constexpr int kIters = 20;

  const Input input = make_input(kRuns);
  const uint64_t expected = expected_sum(input.values, input.lengths);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = run_length_sum(input.values, input.lengths, kIters);
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
