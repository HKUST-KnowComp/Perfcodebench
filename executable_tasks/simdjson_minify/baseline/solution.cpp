#include "interface.h"

#include <nlohmann/json.hpp>

std::string minify_json(const std::string& json_input) {
  return nlohmann::ordered_json::parse(json_input).dump();
}
