#include "interface.h"

#include <rapidjson/writer.h>

#include <cstdint>
#include <string>
#include <vector>

namespace {

class StringBufferStream {
 public:
  explicit StringBufferStream(std::string& out) : out_(out) {}

  void Put(char c) { out_.push_back(c); }
  void Flush() {}

 private:
  std::string& out_;
};

static inline std::size_t count_escaped_json_string_size(const std::string& s) {
  std::size_t n = 0;
  for (unsigned char c : s) {
    switch (c) {
      case '"':
      case '\\':
      case '\b':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
        n += 2;
        break;
      default:
        n += (c < 0x20u) ? 6 : 1;
        break;
    }
  }
  return n;
}

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t reserve_size = 2;  // [ ]
  if (n != 0) {
    reserve_size += n - 1;  // commas between objects
  }

  for (std::size_t i = 0; i < n; ++i) {
    reserve_size += 30;  // punctuation + keys + boolean text + quotes around name

    uint32_t x = ids[i];
    do {
      ++reserve_size;
      x /= 10;
    } while (x != 0);

    x = values[i];
    do {
      ++reserve_size;
      x /= 10;
    } while (x != 0);

    reserve_size += count_escaped_json_string_size(names[i]);
  }

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
