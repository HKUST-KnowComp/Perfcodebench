#include "interface.h"

#include <rapidjson/writer.h>

#include <cstdint>
#include <string>
#include <vector>

namespace {

class StringBufferStream {
 public:
  using Ch = char;

  explicit StringBufferStream(std::string& s) : s_(s) {}

  void Put(char c) { s_.push_back(c); }
  void Flush() {}

 private:
  std::string& s_;
};

static inline std::size_t CountDigits32(uint32_t v) {
  if (v >= 1000000000u) return 10;
  if (v >= 100000000u) return 9;
  if (v >= 10000000u) return 8;
  if (v >= 1000000u) return 7;
  if (v >= 100000u) return 6;
  if (v >= 10000u) return 5;
  if (v >= 1000u) return 4;
  if (v >= 100u) return 3;
  if (v >= 10u) return 2;
  return 1;
}

static std::size_t EstimateSize(const std::vector<uint32_t>& ids,
                                const std::vector<uint32_t>& values,
                                const std::vector<std::string>& names) {
  const std::size_t n = ids.size();
  if (n == 0) return 2;

  std::size_t total = 2 + (n - 1);  // [ ] and commas between objects
  for (std::size_t i = 0; i < n; ++i) {
    total += 35;  // punctuation + keys + bool literal baseline with true
    total += CountDigits32(ids[i]);
    total += CountDigits32(values[i]);
    total += names[i].size();

    // Adjust for false vs true.
    if ((ids[i] % 3u) != 0u) total += 1;

    // Account for escaping in names conservatively for common special chars.
    for (char c : names[i]) {
      if (c == '"' || c == '\\' || static_cast<unsigned char>(c) < 0x20) {
        total += 1;  // minimum extra; control chars may need more, reserve is heuristic only
      }
    }
  }
  return total;
}

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  std::string out;
  out.reserve(EstimateSize(ids, values, names));

  StringBufferStream stream(out);
  rapidjson::Writer<StringBufferStream> writer(stream);

  writer.StartArray();
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();

    writer.Key("id", 2);
    writer.Uint(ids[i]);

    writer.Key("flag", 4);
    writer.Bool((ids[i] % 3u) == 0u);

    writer.Key("value", 5);
    writer.Uint(values[i]);

    writer.Key("name", 4);
    writer.String(names[i].data(), static_cast<rapidjson::SizeType>(names[i].size()));

    writer.EndObject();
  }
  writer.EndArray();

  return out;
}
