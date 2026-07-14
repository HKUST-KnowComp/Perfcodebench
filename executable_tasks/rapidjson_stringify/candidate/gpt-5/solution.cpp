#include "interface.h"

#include <rapidjson/writer.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace {
// Custom OutputStream for RapidJSON that writes directly into std::string
struct StringOutputStream {
  using Ch = char;
  explicit StringOutputStream(std::string& out) : s(out) {}
  void Put(Ch c) { s.push_back(c); }
  void PutN(Ch c, size_t n) { s.append(n, c); }
  void Flush() {}

 private:
  std::string& s;
};
}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Pre-reserve estimated capacity: base overhead per object plus names' sizes
  std::size_t total_name_len = 0;
  total_name_len = 0;
  for (const auto& nm : names) total_name_len += nm.size();
  std::string out;
  out.reserve(2 + total_name_len + n * 50); // [] + names + per-object overhead

  StringOutputStream os(out);
  rapidjson::Writer<StringOutputStream> writer(os);

  writer.StartArray();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();

    // "id": <uint>
    writer.Key("id", 2, false);
    writer.Uint(ids[i]);

    // "flag": <bool>
    writer.Key("flag", 4, false);
    writer.Bool((ids[i] % 3U) == 0U);

    // "value": <uint>
    writer.Key("value", 5, false);
    writer.Uint(values[i]);

    // "name": "<string>"
    writer.Key("name", 4, false);
    const std::string& nm = names[i];
    writer.String(nm.data(), static_cast<rapidjson::SizeType>(nm.size()), false);

    writer.EndObject();
  }
  writer.EndArray();

  return out;
}
