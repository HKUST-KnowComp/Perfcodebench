#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> ids;
  std::vector<uint32_t> ts;
  std::vector<uint16_t> levels;
  std::vector<uint16_t> lens;
};

Input make_input(std::size_t n) {
  Input out;
  out.ids.resize(n);
  out.ts.resize(n);
  out.levels.resize(n);
  out.lens.resize(n);
  uint64_t state = 0x0fedcba987654321ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.ids[i] = static_cast<uint32_t>(state);
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.ts[i] = static_cast<uint32_t>(state);
    out.levels[i] = static_cast<uint16_t>(state & 7u);
    out.lens[i] = static_cast<uint16_t>((state >> 8) & 255u);
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

void write_u32(std::vector<uint8_t>& out, uint32_t v) {
  out.push_back(static_cast<uint8_t>(v));
  out.push_back(static_cast<uint8_t>(v >> 8));
  out.push_back(static_cast<uint8_t>(v >> 16));
  out.push_back(static_cast<uint8_t>(v >> 24));
}

void write_u16(std::vector<uint8_t>& out, uint16_t v) {
  out.push_back(static_cast<uint8_t>(v));
  out.push_back(static_cast<uint8_t>(v >> 8));
}

uint64_t expected_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens) {
  std::vector<uint8_t> out;
  out.reserve(ids.size() * 12);
  for (std::size_t i = 0; i < ids.size(); ++i) {
    write_u32(out, ids[i]);
    write_u32(out, ts[i]);
    write_u16(out, levels[i]);
    write_u16(out, lens[i]);
  }
  return checksum_bytes(out);
}

}  // namespace

int main() {
    const Input input = make_input(3145728);
    constexpr int kIters = 5;
  const uint64_t expected =
      expected_checksum(input.ids, input.ts, input.levels, input.lens);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      serialize_records_checksum(input.ids, input.ts, input.levels, input.lens, kIters);
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
