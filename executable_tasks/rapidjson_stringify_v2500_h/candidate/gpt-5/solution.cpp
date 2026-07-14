#include "interface.h"

#include <rapidjson/writer.h>

namespace {
// Minimal output stream for RapidJSON Writer that appends to a std::string
class StringOutputStream {
public:
    using Ch = char;
    explicit StringOutputStream(std::string& s) : s_(s) {}
    void Put(Ch c) { s_.push_back(c); }
    void Flush() {}
private:
    std::string& s_;
};
}

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Pre-reserve to reduce reallocations: total name length + per-item overhead
  std::size_t names_total = 0;
  for (const auto& s : names) names_total += s.size();
  std::string out;
  out.reserve(names_total + n * 64 + 2);

  StringOutputStream os(out);
  rapidjson::Writer<StringOutputStream> writer(os);

  writer.StartArray();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();

    writer.Key("id", 2);
    writer.Uint(ids[i]);

    writer.Key("flag", 4);
    writer.Bool((ids[i] % 3U) == 0U);

    writer.Key("value", 5);
    writer.Uint(values[i]);

    writer.Key("name", 4);
    const std::string& nm = names[i];
    writer.String(nm.data(), static_cast<rapidjson::SizeType>(nm.size()));

    writer.EndObject();
  }
  writer.EndArray();

  return out;
}
