#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> dim_ts;
  std::vector<uint32_t> dim_values;
  std::vector<uint32_t> probe_ts;
  std::vector<uint32_t> probe_weights;
};

Input make_input(std::size_t dim_size, std::size_t probe_size) {
  Input out;
  out.dim_ts.resize(dim_size);
  out.dim_values.resize(dim_size);
  out.probe_ts.resize(probe_size);
  out.probe_weights.resize(probe_size);
  uint32_t ts = 0;
  for (std::size_t i = 0; i < dim_size; ++i) {
    ts += static_cast<uint32_t>((i & 7u) + 1u);
    out.dim_ts[i] = ts;
    out.dim_values[i] = static_cast<uint32_t>(((i * 17u) + 13u) & 4095u);
  }
  uint64_t state = 0x123456789abcdef0ULL;
  for (std::size_t i = 0; i < probe_size; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.probe_ts[i] = static_cast<uint32_t>(1u + (state % ts));
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.probe_weights[i] = static_cast<uint32_t>((state & 31u) + 1u);
  }
  std::sort(out.probe_ts.begin(), out.probe_ts.end());
  return out;
}

uint64_t expected_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < probe_ts.size(); ++i) {
    const auto it = std::upper_bound(dim_ts.begin(), dim_ts.end(), probe_ts[i]);
    const std::size_t idx = static_cast<std::size_t>(it - dim_ts.begin() - 1);
    sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kDimSize = 1u << 19;
  constexpr std::size_t kProbeSize = 1u << 25;
  constexpr int kIters = 4;

  const Input input = make_input(kDimSize, kProbeSize);
  const uint64_t expected =
      expected_sum(input.dim_ts, input.dim_values, input.probe_ts, input.probe_weights);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      asof_join_sum(input.dim_ts, input.dim_values, input.probe_ts, input.probe_weights, kIters);
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
