#include "interface.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

bool is_private_ip(const uint8_t* ip) {
  return ip[0] == 10u ||
         (ip[0] == 172u && ip[1] >= 16u && ip[1] < 32u) ||
         (ip[0] == 192u && ip[1] == 168u);
}

std::vector<uint8_t> make_packets(std::size_t count) {
  std::vector<uint8_t> out(count * kPacketSize);
  uint64_t state = 0x84222325cbf29ce4ULL;
  for (std::size_t i = 0; i < count; ++i) {
    uint8_t* pkt = out.data() + i * kPacketSize;
    for (std::size_t j = 0; j < kPacketSize; ++j) {
      state = state * 6364136223846793005ULL + 1442695040888963407ULL;
      pkt[j] = static_cast<uint8_t>(state >> 56);
    }
    pkt[0] = 0x45u;
    pkt[8] = static_cast<uint8_t>(1u + (state & 0xFEu));
    pkt[9] = (i % 3u == 0u) ? 6u : ((i % 3u == 1u) ? 17u : 1u);
    if ((i & 1u) == 0u) {
      pkt[12] = 10u;
      pkt[13] = static_cast<uint8_t>(state >> 8);
    } else {
      pkt[12] = 44u;
      pkt[13] = static_cast<uint8_t>(state >> 16);
    }
    const uint16_t dst_port = static_cast<uint16_t>(state & 0xFFFFu);
    pkt[22] = static_cast<uint8_t>(dst_port >> 8);
    pkt[23] = static_cast<uint8_t>(dst_port);
  }
  return out;
}

uint64_t expected_checksum(const std::vector<uint8_t>& packets, std::size_t count) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < count; ++i) {
    const uint8_t* pkt = packets.data() + i * kPacketSize;
    const uint8_t proto = pkt[9];
    const uint8_t ttl = pkt[8];
    const uint16_t dst_port =
        static_cast<uint16_t>((static_cast<uint16_t>(pkt[22]) << 8) | pkt[23]);
    const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
    const uint32_t port_bucket =
        (proto == 6u || proto == 17u) ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u)) : 3u;
    const uint32_t key =
        static_cast<uint32_t>(proto) * 131u + ttl_bucket * 17u +
        (is_private_ip(pkt + 12) ? 5u : 11u) + port_bucket * 23u;
    hash ^= static_cast<uint64_t>(key);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kPacketCount = 1024;
  constexpr int kIters = 192;
  const std::vector<uint8_t> packets = make_packets(kPacketCount);
  const uint64_t expected = expected_checksum(packets, kPacketCount);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = classify_packets_checksum(packets, kPacketCount, kIters);
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
