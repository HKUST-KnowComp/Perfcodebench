#include "interface.h"

#include <rapidjson/writer.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace {
// Minimal RapidJSON-compatible output stream that appends to std::string
struct StringOutputStream {
  using Ch = char;
  std::string& s;
  explicit StringOutputStream(std::string& str) : s(str) {}
  void Put(char c) { s.push_back(c); }
  void Flush() {}
};
}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Pre-reserve output size to minimize reallocations.
  // Estimate: sum of name lengths + ~80 bytes per object for punctuation, keys, numbers, and booleans.
  std::size_t names_len_sum = 0;
  for (const auto& nm : names) names_len_sum += nm.size();
  std::string out;
  out.reserve(names_len_sum + n * 80 + 2); // +2 for []

  StringOutputStream os(out);
  rapidjson::Writer<StringOutputStream> writer(os);

  writer.StartArray();

  const uint32_t* ids_ptr = ids.data();
  const uint32_t* vals_ptr = values.data();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();

    writer.Key("id", 2);
    writer.Uint(ids_ptr[i]);

    writer.Key("flag", 4);
    writer.Bool((ids_ptr[i] % 3U) == 0U);

    writer.Key("value", 5);
    writer.Uint(vals_ptr[i]);

    writer.Key("name", 4);
    const std::string& nm = names[i];
    writer.String(nm.data(), static_cast<rapidjson::SizeType>(nm.size()));

    writer.EndObject();
  }

  writer.EndArray();

  return out;
}
