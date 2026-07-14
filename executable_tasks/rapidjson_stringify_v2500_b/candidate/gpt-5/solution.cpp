#include "interface.h"

#include <rapidjson/writer.h>
#include <string>
#include <vector>

namespace {
struct StringOutStream {
  using Ch = char;
  explicit StringOutStream(std::string& out) : out_(out) {}
  void Put(Ch c) { out_.push_back(c); }
  void Flush() {}
private:
  std::string& out_;
};
}

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Pre-reserve to reduce reallocations: sum of name sizes plus per-item overhead
  std::size_t names_total = 0;
  for (const auto& nm : names) {
    names_total += nm.size();
  }
  std::string result;
  result.reserve(names_total + n * 48 + 2);

  StringOutStream sos(result);
  rapidjson::Writer<StringOutStream> writer(sos);

  writer.StartArray();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();

    writer.Key("id");
    writer.Uint(ids[i]);

    writer.Key("flag");
    writer.Bool((ids[i] % 3U) == 0U);

    writer.Key("value");
    writer.Uint(values[i]);

    writer.Key("name");
    const std::string& nm = names[i];
    writer.String(nm.data(), static_cast<rapidjson::SizeType>(nm.size()));

    writer.EndObject();
  }
  writer.EndArray();

  return result;
}
