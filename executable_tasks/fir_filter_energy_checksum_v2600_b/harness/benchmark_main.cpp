#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<int16_t> make_signal(std::size_t n) {
  std::vector<int16_t> out(n);
  uint64_t state = 0xabcdef1234567890ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out[i] = static_cast<int16_t>((state >> 48) & 0x7FFFu);
    out[i] -= 16384;
  }
  return out;
}

std::vector<int16_t> make_taps(std::size_t n) {
  std::vector<int16_t> out(n);
  uint64_t state = 0x1020304050607080ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out[i] = static_cast<int16_t>((state >> 49) & 0x1FFFu);
    out[i] -= 2048;
  }
  return out;
}

uint64_t expected_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps) {
  uint64_t total = 1469598103934665603ULL;
  for (std::size_t i = 0; i < signal.size(); ++i) {
    int64_t acc = 0;
    const std::size_t max_j = (i + 1u < taps.size()) ? i : (taps.size() - 1u);
    for (std::size_t j = 0; j <= max_j; ++j) {
      acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
    }
    total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
    total *= 1099511628211ULL;
  }
  return total;
}

}  // namespace

int main() {
  constexpr std::size_t kSignal = 65536;
  constexpr std::size_t kTaps = 16;
  constexpr int kIters = 12;
  const std::vector<int16_t> signal = make_signal(kSignal);
  const std::vector<int16_t> taps = make_taps(kTaps);
  const uint64_t expected = expected_checksum(signal, taps);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = fir_energy_checksum(signal, taps, kIters);
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
