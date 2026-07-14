#include "interface.h"

#include <rapidjson/writer.h>

#include <cstdint>
#include <string>
#include <vector>

namespace {

class StringBuffer {
 public:
  StringBuffer() = default;

  void Put(char c) { buffer_.push_back(c); }
  void Flush() {}

  void Reserve(std::size_t n) { buffer_.reserve(n); }

  const std::string& GetString() const { return buffer_; }
  std::string&& MoveString() { return std::move(buffer_); }

 private:
  std::string buffer_;
};

static inline std::size_t count_digits_u32(uint32_t v) {
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

static inline std::size_t estimate_size(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();
  if (n == 0) return 2;

  std::size_t total = 2 + (n - 1);  // [ ] and commas between objects
  for (std::size_t i = 0; i < n; ++i) {
    total += 35;  // punctuation + keys + boolean literal baseline
    total += count_digits_u32(ids[i]);
    total += count_digits_u32(values[i]);
    total += names[i].size() + 8;  // rough extra room for escaping
  }
  return total;
}

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  StringBuffer buffer;
  buffer.Reserve(estimate_size(ids, values, names));

  rapidjson::Writer<StringBuffer> writer(buffer);
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
  return buffer.MoveString();
}
