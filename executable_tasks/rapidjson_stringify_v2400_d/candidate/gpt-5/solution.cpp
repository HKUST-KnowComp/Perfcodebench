#include "interface.h"

#include <rapidjson/writer.h>
#include <cstdint>
#include <string>
#include <vector>

// Minimal output stream for RapidJSON that appends directly to std::string
struct StringOutputStream {
  using Ch = char;
  explicit StringOutputStream(std::string& s) : s_(s) {}
  void Put(Ch c) { s_.push_back(c); }
  void Flush() {}
  void PutN(Ch c, size_t n) { s_.append(n, c); }
private:
  std::string& s_;
};

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Pre-reserve output capacity to minimize reallocations
  std::size_t names_total = 0;
  const std::size_t names_count = names.size() < n ? names.size() : n;
  for (std::size_t i = 0; i < names_count; ++i) names_total += names[i].size();
  std::string out;
  // Rough per-object overhead plus name content
  out.reserve(2 + n * 64 + names_total);

  StringOutputStream sos(out);
  rapidjson::Writer<StringOutputStream> writer(sos);

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
