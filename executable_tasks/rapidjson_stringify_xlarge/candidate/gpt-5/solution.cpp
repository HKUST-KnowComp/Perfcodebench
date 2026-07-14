#include "interface.h"

#include <rapidjson/writer.h>

#include <cstdint>
#include <string>
#include <vector>

// Minimal RapidJSON-compatible output stream that writes directly into a std::string
class StringRefStream {
public:
  using Ch = char;
  explicit StringRefStream(std::string& out) : out_(out) {}
  void Put(char c) { out_.push_back(c); }
  void Flush() {}
private:
  std::string& out_;
};

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Pre-reserve output capacity to reduce reallocations
  std::size_t names_bytes = 0;
  names_bytes.reserve(n); // avoid repeated realloc in accumulation
  for (const auto& s : names) names_bytes += s.size();
  std::size_t approx = names_bytes + (n * 64u) + 2u; // rough overhead estimate

  std::string out;
  out.reserve(approx);

  StringRefStream os(out);
  rapidjson::Writer<StringRefStream> writer(os);

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
    const std::string& name = names[i];
    writer.String(name.data(), static_cast<rapidjson::SizeType>(name.size()));

    writer.EndObject();
  }
  writer.EndArray();

  return out;
}
