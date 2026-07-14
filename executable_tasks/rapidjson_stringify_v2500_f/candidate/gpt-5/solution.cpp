#include "interface.h"

#include <rapidjson/writer.h>
#include <string>
#include <cstddef>

namespace {
struct StringOutputStream {
  using Ch = char;
  std::string s;
  inline void Put(Ch c) { s.push_back(c); }
  inline void PutN(Ch c, std::size_t n) { s.append(n, c); }
  inline void Flush() {}
};
}

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  StringOutputStream os;
  std::size_t names_len = 0;
  for (const auto& nm : names) names_len += nm.size();
  std::size_t reserve = 2 + n * 64 + names_len * 2; // rough pre-reservation
  os.s.reserve(reserve);

  rapidjson::Writer<StringOutputStream> writer(os);
  writer.StartArray();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();

    writer.Key("id", 2);
    writer.Uint(static_cast<unsigned>(ids[i]));

    writer.Key("flag", 4);
    bool flag = (ids[i] % 3U) == 0U;
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
