#include "interface.h"

#include <rapidjson/writer.h>

#include <cstdint>
#include <string>
#include <vector>

namespace {

class StringBufferStream {
 public:
  using Ch = char;

  explicit StringBufferStream(std::string& out) : out_(out) {}

  void Put(char c) { out_.push_back(c); }
  void Flush() {}

  char* PutBegin() { return nullptr; }
  std::size_t PutEnd(char*) { return 0; }

 private:
  std::string& out_;
};

inline std::size_t CountDigits(uint32_t v) {
  if (v < 10U) return 1;
  if (v < 100U) return 2;
  if (v < 1000U) return 3;
  if (v < 10000U) return 4;
  if (v < 100000U) return 5;
  if (v < 1000000U) return 6;
  if (v < 10000000U) return 7;
  if (v < 100000000U) return 8;
  if (v < 1000000000U) return 9;
  return 10;
}

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t reserve_size = 2;  // [ ]
  for (std::size_t i = 0; i < n; ++i) {
    reserve_size += 35;  // punctuation + keys + bool literal baseline
    reserve_size += CountDigits(ids[i]);
    reserve_size += CountDigits(values[i]);
    reserve_size += names[i].size() + 2;  // quotes around name; escaping may grow further
  }
  if (n > 1) reserve_size += (n - 1);

  std::string out;
  out.reserve(reserve_size);

  StringBufferStream stream(out);
  rapidjson::Writer<StringBufferStream> writer(stream);

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
    writer.String(names[i].data(), static_cast<rapidjson::SizeType>(names[i].size()));

    writer.EndObject();
  }
  writer.EndArray();

  return out;
}
