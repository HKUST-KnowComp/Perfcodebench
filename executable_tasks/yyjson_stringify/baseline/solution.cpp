#include "interface.h"

#include <nlohmann/json.hpp>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  nlohmann::ordered_json arr = nlohmann::ordered_json::array();
  for (std::size_t i = 0; i < ids.size(); ++i) {
    nlohmann::ordered_json obj;
    obj["id"] = ids[i];
    obj["flag"] = ((ids[i] % 3U) == 0U);
    obj["value"] = values[i];
    obj["name"] = names[i];
    arr.push_back(std::move(obj));
  }
  return arr.dump();
}
