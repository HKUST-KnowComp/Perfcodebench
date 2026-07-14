#include "interface.h"

#include <simdjson.h>

#include <string>
#include <vector>
#include <cstring>

namespace {

static inline void append_escaped_json_string(std::string &out, std::string_view s) {
  out.push_back('"');
  const char *p = s.data();
  const char *end = p + s.size();
  const char *chunk = p;

  while (p < end) {
    unsigned char c = static_cast<unsigned char>(*p);
    if (c == '"' || c == '\\' || c <= 0x1F) {
      if (chunk < p) out.append(chunk, static_cast<size_t>(p - chunk));
      switch (c) {
        case '"': out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\b': out += "\\b"; break;
        case '\f': out += "\\f"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default: {
          static constexpr char hex[] = "0123456789abcdef";
          char buf[6] = {'\\', 'u', '0', '0', hex[c >> 4], hex[c & 0x0F]};
          out.append(buf, 6);
          break;
        }
      }
      ++p;
      chunk = p;
    } else {
      ++p;
    }
  }
  if (chunk < end) out.append(chunk, static_cast<size_t>(end - chunk));
  out.push_back('"');
}

static void minify_element(simdjson::ondemand::value val, std::string &out);

static void minify_array(simdjson::ondemand::array arr, std::string &out) {
  out.push_back('[');
  bool first = true;
  for (simdjson::ondemand::value elem : arr) {
    if (!first) out.push_back(',');
    first = false;
    minify_element(elem, out);
  }
  out.push_back(']');
}

static void minify_object(simdjson::ondemand::object obj, std::string &out) {
  out.push_back('{');
  bool first = true;
  for (simdjson::ondemand::field field : obj) {
    if (!first) out.push_back(',');
    first = false;

    std::string_view key = field.unescaped_key();
    append_escaped_json_string(out, key);
    out.push_back(':');
    minify_element(field.value(), out);
  }
  out.push_back('}');
}

static void minify_element(simdjson::ondemand::value val, std::string &out) {
  using simdjson::ondemand::json_type;
  switch (val.type()) {
    case json_type::object:
      minify_object(val.get_object(), out);
      return;
    case json_type::array:
      minify_array(val.get_array(), out);
      return;
    case json_type::string: {
      std::string_view s = val.get_string();
      append_escaped_json_string(out, s);
      return;
    }
    case json_type::number: {
      std::string_view raw = val.raw_json_token();
      out.append(raw.data(), raw.size());
      return;
    }
    case json_type::boolean: {
      bool b = val.get_bool();
      out += b ? "true" : "false";
      return;
    }
    case json_type::null:
      out += "null";
      return;
  }
}

} // namespace

std::string minify_json(const std::string& json_input) {
  simdjson::ondemand::parser parser;

  std::vector<char> padded(json_input.size() + simdjson::SIMDJSON_PADDING);
  if (!json_input.empty()) {
    std::memcpy(padded.data(), json_input.data(), json_input.size());
  }
  std::memset(padded.data() + json_input.size(), 0, simdjson::SIMDJSON_PADDING);

  std::string out;
  out.reserve(json_input.size());

  auto doc_result = parser.iterate(padded.data(), json_input.size(), padded.size());
  if (doc_result.error()) {
    return {};
  }
  simdjson::ondemand::document doc = doc_result.value();
  minify_element(doc, out);
  return out;
}
