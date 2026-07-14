#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> timestamps;
  std::vector<uint32_t> ids;
  std::vector<uint16_t> levels;
  std::vector<std::string> messages;
};

Input make_input(std::size_t n) {
  Input out;
  out.timestamps.resize(n);
  out.ids.resize(n);
  out.levels.resize(n);
  out.messages.resize(n);
  uint64_t state = 0xabcdef0123456789ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.timestamps[i] = static_cast<uint32_t>(state);
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.ids[i] = static_cast<uint32_t>(state);
    out.levels[i] = static_cast<uint16_t>(state & 7u);
    const std::size_t len = 8 + static_cast<std::size_t>((state >> 12) & 31u);
    out.messages[i].reserve(len);
    for (std::size_t j = 0; j < len; ++j) {
      state = state * 2862933555777941757ULL + 3037000493ULL;
      out.messages[i].push_back(static_cast<char>('a' + (state % 26ULL)));
    }
  }
  return out;
}

uint64_t checksum_text(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char c : text) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void append_u32(std::string& out, uint32_t v) {
  char buf[16];
  int n = 0;
  do {
    buf[n++] = static_cast<char>('0' + (v % 10u));
    v /= 10u;
  } while (v != 0u);
  while (n--) {
    out.push_back(buf[n]);
  }
}

uint64_t expected_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages) {
  std::string out;
  for (std::size_t i = 0; i < ids.size(); ++i) {
    append_u32(out, timestamps[i]);
    out.push_back('|');
    append_u32(out, ids[i]);
    out.push_back('|');
    append_u32(out, levels[i]);
    out.push_back('|');
    out.append(messages[i]);
    out.push_back('\n');
  }
  return checksum_text(out);
}

}  // namespace

int main() {
    const Input input = make_input(1u << 21);
    constexpr int kIters = 4;
  const uint64_t expected =
      expected_checksum(input.timestamps, input.ids, input.levels, input.messages);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      batched_log_checksum(input.timestamps, input.ids, input.levels, input.messages, kIters);
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
