#include "interface.h"

#include <rapidjson/writer.h>
#include <cstdint>
#include <string>
#include <vector>

// Minimal RapidJSON OutputStream that appends directly into a std::string
struct StringOutputStream {
  using Ch = char;
  explicit StringOutputStream(std::string& out) : s_(out) {}
  void Put(Ch c) { s_.push_back(c); }
  void Flush() {}
private:
  std::string& s_;
};

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Pre-reserve an estimated capacity to minimize reallocations
  std::size_t names_total = 0;
  for (std::size_t i = 0; i < n; ++i) names_total += names[i].size();
  std::string out;
  // Base estimate: per object overhead + names length
  // 48 bytes per object is a conservative overhead for syntax, keys, booleans and numbers
  out.reserve(2 + n * 48 + names_total);

  StringOutputStream sos(out);
  rapidjson::Writer<StringOutputStream> writer(sos);

  writer.StartArray();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();

    writer.Key("id", static_cast<rapidjson::SizeType>(2));
    writer.Uint(ids[i]);

    writer.Key("flag", static_cast<rapidjson::SizeType>(4));
    const bool flag = ((ids[i] % 3U) == 0U);
    writer.Bool(flag);

    writer.Key("value", static_cast<rapidjson::SizeType>(5));
    writer.Uint(values[i]);

    writer.Key("name", static_cast<rapidjson::SizeType>(4));
    const std::string& nm = names[i];
    writer.String(nm.data(), static_cast<rapidjson::SizeType>(nm.size()));

    writer.EndObject();
  }
  writer.EndArray();

  return out;
}
