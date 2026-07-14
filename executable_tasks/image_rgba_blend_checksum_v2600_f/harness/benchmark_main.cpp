#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint8_t> src;
  std::vector<uint8_t> dst;
};

Input make_input(std::size_t pixels) {
  Input out;
  out.src.resize(pixels * 4u);
  out.dst.resize(pixels * 4u);
  uint64_t state = 0x123456789abcdef0ULL;
  for (std::size_t i = 0; i < pixels; ++i) {
    for (int c = 0; c < 3; ++c) {
      state = state * 6364136223846793005ULL + 1442695040888963407ULL;
      out.src[i * 4u + static_cast<std::size_t>(c)] = static_cast<uint8_t>(state >> 56);
      state = state * 2862933555777941757ULL + 3037000493ULL;
      out.dst[i * 4u + static_cast<std::size_t>(c)] = static_cast<uint8_t>(state >> 56);
    }
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.src[i * 4u + 3u] = static_cast<uint8_t>(state >> 56);
    out.dst[i * 4u + 3u] = 255u;
  }
  return out;
}

uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * a +
                               static_cast<uint32_t>(d) * (255u - a) + 127u) / 255u);
}

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t expected_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height) {
  std::vector<uint8_t> out(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4u);
  const std::size_t pixels = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
  for (std::size_t i = 0; i < pixels; ++i) {
    const std::size_t idx = i * 4u;
    const uint8_t a = src[idx + 3u];
    out[idx] = blend_byte(src[idx], dst[idx], a);
    out[idx + 1u] = blend_byte(src[idx + 1u], dst[idx + 1u], a);
    out[idx + 2u] = blend_byte(src[idx + 2u], dst[idx + 2u], a);
    out[idx + 3u] = 255u;
  }
  return checksum_bytes(out);
}

}  // namespace

int main() {
  constexpr int kWidth = 128;
  constexpr int kHeight = 2048;
  constexpr int kIters = 6;
  const Input input = make_input(static_cast<std::size_t>(kWidth) * static_cast<std::size_t>(kHeight));
  const uint64_t expected = expected_checksum(input.src, input.dst, kWidth, kHeight);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = blend_checksum(input.src, input.dst, kWidth, kHeight, kIters);
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
