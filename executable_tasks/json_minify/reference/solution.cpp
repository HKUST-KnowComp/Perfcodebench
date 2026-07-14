#include "interface.h"

#include "simdjson.h"

std::string minify_json(const std::string& json_input) {
  simdjson::padded_string padded(json_input);
  std::string out;
  out.resize(json_input.size());
  size_t out_len = 0;
  const auto error = simdjson::get_active_implementation()->minify(
      reinterpret_cast<const uint8_t*>(padded.data()),
      padded.length(),
      reinterpret_cast<uint8_t*>(out.data()),
      out_len);
  if (error) {
    return {};
  }
  out.resize(out_len);
  return out;
}
