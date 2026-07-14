#include "interface.h"

#include <rapidjson/writer.h>

#include <cstdint>
#include <string>
#include <vector>

namespace {
class StringBuffer {
 public:
  StringBuffer() { data_.reserve(256); }

  void Put(char c) { data_.push_back(c); }
  void Flush() {}

  void Reserve(std::size_t n) { data_.reserve(n); }

  const char* GetString() const { return data_.c_str(); }
  std::size_t GetSize() const { return data_.size(); }

  std::string&& MoveString() { return std::move(data_); }

 private:
  std::string data_;
};

static std::size_t CountDigits(uint32_t v) {
  if (v >= 1000000000U) return 10;
  if (v >= 100000000U) return 9;
  if (v >= 10000000U) return 8;
  if (v >= 1000000U) return 7;
  if (v >= 100000U) return 6;
  if (v >= 10000U) return 5;
  if (v >= 1000U) return 4;
  if (v >= 100U) return 3;
  if (v >= 10U) return 2;
  return 1;
}
}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t reserve = 2;
  for (std::size_t i = 0; i < n; ++i) {
    reserve += 36;
    reserve += CountDigits(ids[i]);
    reserve += CountDigits(values[i]);
    reserve += names[i].size();
  }

  StringBuffer buffer;
  buffer.Reserve(reserve);

  rapidjson::Writer<StringBuffer> writer(buffer);
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

  return buffer.MoveString();
}
