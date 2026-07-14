#include "interface.h"

#include <nlohmann/json.hpp>

#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  const auto doc = nlohmann::json::parse(json_input);
  uint64_t sum = 0;
  for (const auto& item : doc) {
    if (!item["flag"].get<bool>()) {
      continue;
    }
    const std::string name = item["name"].get<std::string>();
    sum += item["id"].get<uint64_t>();
    sum += item["value"].get<uint64_t>();
    sum += static_cast<uint64_t>(name.size());
  }
  return sum;
}
