#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> padded_tokens;
  std::vector<int> lengths;
};

Input make_input(int batch, int max_len) {
  Input out;
  out.padded_tokens.resize(static_cast<std::size_t>(batch) * static_cast<std::size_t>(max_len), 0);
  out.lengths.resize(static_cast<std::size_t>(batch));
  uint64_t state = 0xabcdef0123456789ULL;
  for (int b = 0; b < batch; ++b) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    const int len = 32 + static_cast<int>(state % static_cast<uint64_t>(max_len - 31));
    out.lengths[static_cast<std::size_t>(b)] = len;
    for (int i = 0; i < len; ++i) {
      state = state * 6364136223846793005ULL + 1442695040888963407ULL;
      out.padded_tokens[static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len) + static_cast<std::size_t>(i)] =
          static_cast<uint32_t>((state & 32767u) + 1u);
    }
  }
  return out;
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t expected_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len) {
  uint64_t hash = 1469598103934665603ULL;
  for (int b = 0; b < batch; ++b) {
    for (int i = 0; i < lengths[static_cast<std::size_t>(b)]; ++i) {
      const uint32_t token =
          padded_tokens[static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len) + static_cast<std::size_t>(i)];
      hash = mix(hash, token);
      hash = mix(hash, static_cast<uint64_t>(i));
    }
  }
  return hash;
}

}  // namespace

int main() {
  constexpr int kBatch = 4096;
  constexpr int kMaxLen = 256;
  constexpr int kIters = 20;

  const Input input = make_input(kBatch, kMaxLen);
  const uint64_t expected = expected_checksum(input.padded_tokens, input.lengths, kBatch, kMaxLen);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = ragged_pack_checksum(input.padded_tokens, input.lengths, kBatch, kMaxLen, kIters);
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
