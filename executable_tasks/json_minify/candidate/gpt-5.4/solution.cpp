#include "interface.h"

#include <nlohmann/json.hpp>
#include <string>

std::string minify_json(const std::string& json_input) {
  nlohmann::ordered_json j = nlohmann::ordered_json::parse(json_input);
  return j.dump(-1, ' ', false, nlohmann::json::error_handler_t::strict);
}
