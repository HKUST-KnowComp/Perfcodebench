#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> starts;
  std::vector<uint32_t> ends;
  std::vector<uint32_t> payloads;
  std::vector<uint32_t> probes;
  std::vector<uint32_t> weights;
};

Input make_input(std::size_t intervals, std::size_t probes) {
  Input out;
  out.starts.resize(intervals);
  out.ends.resize(intervals);
  out.payloads.resize(intervals);
  out.probes.resize(probes);
  out.weights.resize(probes);
  uint32_t x = 0;
  for (std::size_t i = 0; i < intervals; ++i) {
    const uint32_t len = static_cast<uint32_t>((i & 7u) + 1u);
    out.starts[i] = x;
    out.ends[i] = x + len;
    out.payloads[i] = static_cast<uint32_t>(((i * 19u) + 5u) & 4095u);
    x += len;
  }
  uint64_t state = 0xfeedfacecafebeefULL;
  for (std::size_t i = 0; i < probes; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.probes[i] = static_cast<uint32_t>(state % x);
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.weights[i] = static_cast<uint32_t>((state & 31u) + 1u);
  }
  std::sort(out.probes.begin(), out.probes.end());
  return out;
}

uint64_t expected_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < probes.size(); ++i) {
    const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
    const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
    sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kIntervals = 1u << 19;
  constexpr std::size_t kProbes = 1u << 21;
  constexpr int kIters = 12;

  const Input input = make_input(kIntervals, kProbes);
  const uint64_t expected =
      expected_sum(input.starts, input.payloads, input.probes, input.weights);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = interval_lookup_sum(
      input.starts,
      input.ends,
      input.payloads,
      input.probes,
      input.weights,
      kIters);
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
