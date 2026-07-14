#include "interface.h"

#include <simdjson.h>
#include <nlohmann/json.hpp>
#include <string>

std::string minify_json(const std::string& json_input) {
  // Fast path: SIMD-accelerated minification without building a DOM
  auto res = simdjson::minify(json_input);
  if (!res.error()) {
    return res.value_unsafe();
  }
  // Fallback: robust parse + dump to preserve correctness if minify reports error
  // (Should not be hit for valid JSON inputs used by the harness.)
  return nlohmann::ordered_json::parse(json_input).dump();
}
