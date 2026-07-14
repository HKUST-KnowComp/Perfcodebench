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

  // RapidJSON may call these in some configurations; provide minimal support.
  char* PutBegin() { return nullptr; }
  std::size_t PutEnd(char*) { return 0; }

 private:
  std::string& out_;
};

static inline std::size_t count_escaped_json_string_size(const std::string& s) {
  std::size_t extra = 0;
  for (unsigned char c : s) {
    if (c == '"' || c == '\\') {
      extra += 1;
    } else if (c < 0x20) {
      extra += 5;  // "\u00XX" replaces one byte.
    }
  }
  return s.size() + extra;
}

static inline unsigned decimal_digits_u32(uint32_t v) {
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

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::string out;

  // Reserve a close estimate to reduce reallocations.
  // Per object fixed syntax:
  // {"id":,"flag":,"value":,"name":""}
  // plus commas between objects.
  std::size_t reserve_size = 2;  // [ ]
  if (n > 0) reserve_size += (n - 1);
  for (std::size_t i = 0; i < n; ++i) {
    reserve_size += 33;  // fixed punctuation + keys + true/false baseline room
    reserve_size += decimal_digits_u32(ids[i]);
    reserve_size += ((ids[i] % 3U) == 0U) ? 4 : 5;
    reserve_size += decimal_digits_u32(values[i]);
    reserve_size += count_escaped_json_string_size(names[i]);
  }
  out.reserve(reserve_size);

  StringBufferStream os(out);
  rapidjson::Writer<StringBufferStream> writer(os);

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
