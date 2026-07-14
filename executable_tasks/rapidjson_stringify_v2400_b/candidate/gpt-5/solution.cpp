#include "interface.h"

#include <rapidjson/writer.h>

namespace {
struct StringOutputStream {
  using Ch = char;
  std::string s;
  void Put(char c) { s.push_back(c); }
  void Flush() {}
  void PutN(char c, size_t n) { s.append(n, c); }
};
inline void PutN(StringOutputStream& stream, char c, size_t n) { stream.PutN(c, n); }
}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  StringOutputStream os;

  // Reserve approximate capacity to reduce reallocations.
  // Base overhead per object (~32) + total name sizes + brackets
  size_t names_total = 0;
  names_total = 0;
  for (const auto& nm : names) names_total += nm.size();
  const size_t n = ids.size();
  const size_t reserve = names_total + n * 32u + 2u;
  os.s.reserve(reserve);

  rapidjson::Writer<StringOutputStream> writer(os);
  writer.StartArray();
  for (size_t i = 0; i < n; ++i) {
    writer.StartObject();

    writer.Key("id", 2);
    writer.Uint(static_cast<unsigned>(ids[i]));

    writer.Key("flag", 4);
    const bool flag = ((ids[i] % 3u) == 0u);
    writer.Bool(flag);

    writer.Key("value", 5);
    writer.Uint(static_cast<unsigned>(values[i]));

    writer.Key("name", 4);
    const std::string& nm = names[i];
    writer.String(nm.data(), static_cast<rapidjson::SizeType>(nm.size()));

    writer.EndObject();
  }
  writer.EndArray();

  return std::move(os.s);
}
