#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> dim_keys;
  std::vector<uint32_t> dim_values;
  std::vector<uint32_t> probe_keys;
  std::vector<uint32_t> probe_weights;
};

Input make_input(std::size_t dim_size, std::size_t probe_size) {
  Input out;
  out.dim_keys.resize(dim_size);
  out.dim_values.resize(dim_size);
  out.probe_keys.resize(probe_size);
  out.probe_weights.resize(probe_size);
  for (std::size_t i = 0; i < dim_size; ++i) {
    out.dim_keys[i] = static_cast<uint32_t>(i);
    out.dim_values[i] = static_cast<uint32_t>((i * 17u + 13u) & 1023u);
  }
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  for (std::size_t i = 0; i < probe_size; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.probe_keys[i] = static_cast<uint32_t>(state & (static_cast<uint64_t>(dim_size) - 1ULL));
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.probe_weights[i] = static_cast<uint32_t>((state & 31u) + 1u);
  }
  return out;
}

uint64_t expected_sum(
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < probe_keys.size(); ++i) {
    sum += static_cast<uint64_t>(dim_values[static_cast<std::size_t>(probe_keys[i])]) *
           static_cast<uint64_t>(probe_weights[i]);
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kDimSize = 1u << 18;
  constexpr std::size_t kProbeSize = 1u << 24;
  constexpr int kIters = 4;

  const Input input = make_input(kDimSize, kProbeSize);
  const uint64_t expected = expected_sum(input.dim_values, input.probe_keys, input.probe_weights);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      join_sum(input.dim_keys, input.dim_values, input.probe_keys, input.probe_weights, kIters);
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
