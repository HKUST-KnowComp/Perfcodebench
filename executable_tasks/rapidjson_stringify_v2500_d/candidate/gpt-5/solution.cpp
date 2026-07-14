#include "interface.h"
#include <rapidjson/writer.h>

namespace {
struct StringOutputStream {
  using Ch = char;
  explicit StringOutputStream(std::string& s) : s_(s) {}
  inline void Put(char c) { s_.push_back(c); }
  inline void Flush() {}
  // Optional optimization used by Writer when available
  inline void PutN(char c, size_t n) { s_.append(n, c); }
private:
  std::string& s_;
};
}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Pre-reserve approximate capacity to minimize reallocations
  std::size_t nameChars = 0;
  for (const auto& s : names) nameChars += s.size();
  std::string out;
  out.reserve(nameChars + n * 64u + 2u);

  StringOutputStream os(out);
  rapidjson::Writer<StringOutputStream> writer(os);

  writer.StartArray();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();

    writer.Key("id", 2, false);
    writer.Uint(static_cast<unsigned>(ids[i]));

    writer.Key("flag", 4, false);
    writer.Bool((ids[i] % 3u) == 0u);

    writer.Key("value", 5, false);
    writer.Uint(static_cast<unsigned>(values[i]));

    writer.Key("name", 4, false);
    const std::string& nm = names[i];
    writer.String(nm.data(), static_cast<rapidjson::SizeType>(nm.size()), false);

    writer.EndObject();
  }
  writer.EndArray();

  return out;
}
