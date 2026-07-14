#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (timestamps.empty()) return 0;

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  // Precompute string representations
  std::vector<std::string> precomputed;
  precomputed.reserve(timestamps.size() * 4);
  for (std::size_t i = 0; i < timestamps.size(); ++i) {
    precomputed.push_back(std::to_string(timestamps[i]));
    precomputed.push_back(std::to_string(ids[i]));
    precomputed.push_back(std::to_string(levels[i]));
    precomputed.push_back(messages[i]);
  }

  // Calculate total size needed
  std::size_t total_size = 0;
  for (std::size_t i = 0; i < timestamps.size(); ++i) {
    total_size += precomputed[i * 4].size() + 1; // timestamp
    total_size += precomputed[i * 4 + 1].size() + 1; // id
    total_size += precomputed[i * 4 + 2].size() + 1; // level
    total_size += precomputed[i * 4 + 3].size() + 1; // message
  }

  std::string out;
  out.reserve(total_size);

  // Build output once
  for (std::size_t i = 0; i < timestamps.size(); ++i) {
    out += precomputed[i * 4];
    out += '|';
    out += precomputed[i * 4 + 1];
    out += '|';
    out += precomputed[i * 4 + 2];
    out += '|';
    out += precomputed[i * 4 + 3];
    out += '\n';
  }

  // Compute checksum once
  for (unsigned char c : out) {
    hash ^= static_cast<uint64_t>(c);
    hash *= FNV_PRIME;
  }

  return hash;
}