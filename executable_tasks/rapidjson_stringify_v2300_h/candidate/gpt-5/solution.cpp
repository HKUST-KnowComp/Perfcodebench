#include "interface.h"

#include <rapidjson/writer.h>
#include <cstdint>
#include <string>
#include <vector>

namespace {
struct StringOutput {
  using Ch = char;
  std::string* s;
  explicit StringOutput(std::string& out) : s(&out) {}
  void Put(Ch c) { s->push_back(c); }
  void Flush() {}
  void PutN(Ch c, size_t n) { s->append(n, c); }
};

static inline size_t sum_names_length(const std::vector<std::string>& names) {
  size_t total = 0;
  for (const auto& nm : names) total += nm.size();
  return total;
}
}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::string out;
  out.reserve(sum_names_length(names) + n * 64 + 2);

  StringOutput os(out);
  rapidjson::Writer<StringOutput> writer(os);
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
