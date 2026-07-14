#include "interface.h"

#include "simdjson.h"

#include <string>

std::string minify_json(const std::string& json_input) {
  std::string out;
  out.resize(json_input.size());
  size_t out_len = 0;
  const auto error = simdjson::minify(json_input.data(), json_input.size(), out.data(), out_len);
  if (error) {
    return {};
  }
  out.resize(out_len);
  return out;
}
