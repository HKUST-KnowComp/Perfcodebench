#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int /*iters*/) {
  uint64_t hash = 1469598103934665603ULL;
  
  auto update = [&hash](char c) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= 1099511628211ULL;
  };
  
  auto feed_uint32 = [&update](uint32_t val) {
    if (val == 0) {
      update('0');
      return;
    }
    char buf[10];
    int pos = 0;
    while (val > 0) {
      buf[pos++] = '0' + (val % 10);
      val /= 10;
    }
    for (int i = pos - 1; i >= 0; --i) {
      update(buf[i]);
    }
  };
  
  auto feed_uint16 = [&update](uint16_t val) {
    if (val == 0) {
      update('0');
      return;
    }
    char buf[5];
    int pos = 0;
    while (val > 0) {
      buf[pos++] = '0' + (val % 10);
      val /= 10;
    }
    for (int i = pos - 1; i >= 0; --i) {
      update(buf[i]);
    }
  };
  
  const std::size_t n = timestamps.size();
  for (std::size_t i = 0; i < n; ++i) {
    feed_uint32(timestamps[i]);
    update('|');
    feed_uint32(ids[i]);
    update('|');
    feed_uint16(levels[i]);
    update('|');
    const std::string& msg = messages[i];
    for (char c : msg) {
      update(c);
    }
    update('\n');
  }
  
  return hash;
}
