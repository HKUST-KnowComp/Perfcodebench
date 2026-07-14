#include "interface.h"

#include <rapidjson/writer.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace {
struct StringOut {
  using Ch = char;
  std::string s;
  void Put(char c) { s.push_back(c); }
  void PutN(char c, size_t n) { s.append(n, c); }
  void Flush() {}
  const char* GetString() const { return s.c_str(); }
  size_t GetSize() const { return s.size(); }
};
}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  StringOut out;

  const std::size_t n = ids.size();
  std::size_t name_bytes = 0;
  for (const auto& nm : names) name_bytes += nm.size();
  // Reserve a rough estimate to minimize reallocations
  // Per object overhead ~64 bytes plus name length
  out.s.reserve(name_bytes + n * 64 + 2);

  rapidjson::Writer<StringOut> w(out);
  w.StartArray();
  for (std::size_t i = 0; i < n; ++i) {
    w.StartObject();

    w.Key("id", 2);
    w.Uint(static_cast<unsigned>(ids[i]));

    w.Key("flag", 4);
    const bool flag = (ids[i] % 3u) == 0u;
    w.Bool(flag);

    w.Key("value", 5);
    w.Uint(static_cast<unsigned>(values[i]));

    w.Key("name", 4);
    const std::string& nm = names[i];
    w.String(nm.data(), static_cast<rapidjson::SizeType>(nm.size()));

    w.EndObject();
  }
  w.EndArray();

  return std::move(out.s);
}
