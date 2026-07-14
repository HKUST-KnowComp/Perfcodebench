#include "interface.h"

#include <rapidjson/writer.h>

#include <string>
#include <vector>
#include <cstdint>

// Minimal RapidJSON-compatible output stream that writes directly into std::string
struct StringOutputStream {
  using Ch = char;
  explicit StringOutputStream(std::string& out) : s(out) {}
  void Put(Ch c) { s.push_back(c); }
  void Flush() {}
  void PutN(Ch c, size_t n) { s.append(n, c); }
  std::string& s;
};

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Pre-reserve an approximate capacity to minimize reallocations
  std::size_t names_total = 0;
  for (std::size_t i = 0; i < n; ++i) names_total += names[i].size();
  // Rough overhead per object (keys, punctuation, worst-case boolean, and numbers);
  // conservative to avoid reallocation
  const std::size_t approx_per_item = 100; // conservative estimate
  std::string out;
  out.reserve(2 + (n > 0 ? (n - 1) : 0) + names_total + n * approx_per_item);

  StringOutputStream os(out);
  rapidjson::Writer<StringOutputStream> writer(os);

  writer.StartArray();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();

    writer.Key("id", static_cast<rapidjson::SizeType>(2));
    writer.Uint(ids[i]);

    writer.Key("flag", static_cast<rapidjson::SizeType>(4));
    writer.Bool((ids[i] % 3U) == 0U);

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
