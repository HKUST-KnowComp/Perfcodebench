#include "interface.h"

#include <nlohmann/json.hpp>

uint64_t compute_checksum(const std::string& json_input) {
  const auto doc = nlohmann::json::parse(json_input);
  uint64_t sum = 0;
  for (const auto& item : doc) {
    sum += item["id"].get<uint64_t>();
    sum += item["nested"]["a"].get<uint64_t>();
    sum += item["nested"]["b"].get<uint64_t>();
  }
  return sum;
}
