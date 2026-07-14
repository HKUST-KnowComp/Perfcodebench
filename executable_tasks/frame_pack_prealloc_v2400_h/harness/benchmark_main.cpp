#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

std::vector<std::string> make_frames(std::size_t n) {
  std::vector<std::string> out(n);
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t len = 8 + static_cast<std::size_t>(state % 56ULL);
    out[i].reserve(len);
    for (std::size_t j = 0; j < len; ++j) {
      state = state * 2862933555777941757ULL + 3037000493ULL;
      out[i].push_back(static_cast<char>('a' + (state % 26ULL)));
    }
  }
  return out;
}

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void write_len(std::vector<uint8_t>& out, uint16_t v) {
  out.push_back(static_cast<uint8_t>(v));
  out.push_back(static_cast<uint8_t>(v >> 8));
}

uint64_t expected_checksum(const std::vector<std::string>& frames) {
  std::vector<uint8_t> out;
  for (const std::string& frame : frames) {
    write_len(out, static_cast<uint16_t>(frame.size()));
    out.insert(out.end(), frame.begin(), frame.end());
  }
  return checksum_bytes(out);
}

}  // namespace

int main() {
    const std::vector<std::string> frames = make_frames(610000);
    constexpr int kIters = 8;
  const uint64_t expected = expected_checksum(frames);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = frame_pack_checksum(frames, kIters);
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
