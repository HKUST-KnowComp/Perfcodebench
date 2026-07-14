#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

namespace {

std::vector<int32_t> make_logits(int beams, int vocab) {
  std::vector<int32_t> out(static_cast<std::size_t>(beams) * static_cast<std::size_t>(vocab));
  uint64_t state = 0x13579bdf2468ace0ULL;
  for (std::size_t i = 0; i < out.size(); ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out[i] = static_cast<int32_t>((state % 50001ULL) - 25000);
  }
  return out;
}

bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t expected_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk) {
  uint64_t hash = 1469598103934665603ULL;
  std::vector<std::pair<int32_t, int>> row(static_cast<std::size_t>(vocab));
  for (int b = 0; b < beams; ++b) {
    const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
    for (int v = 0; v < vocab; ++v) {
      row[static_cast<std::size_t>(v)] = {logits[base + static_cast<std::size_t>(v)], v};
    }
    std::sort(row.begin(), row.end(), better);
    for (int k = 0; k < topk; ++k) {
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[static_cast<std::size_t>(k)].first)));
      hash = mix(hash, static_cast<uint64_t>(row[static_cast<std::size_t>(k)].second));
    }
  }
  return hash;
}

}  // namespace

int main() {
  constexpr int kBeams = 64;
  constexpr int kVocab = 8192;
  constexpr int kTopK = 8;
  constexpr int kIters = 8;

  const std::vector<int32_t> logits = make_logits(kBeams, kVocab);
  const uint64_t expected = expected_checksum(logits, kBeams, kVocab, kTopK);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = beam_topk_checksum(logits, kBeams, kVocab, kTopK, kIters);
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
