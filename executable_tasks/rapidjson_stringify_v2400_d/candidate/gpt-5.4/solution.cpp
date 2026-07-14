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

  char* PutBegin() { return nullptr; }
  std::size_t PutEnd(char*) { return 0; }

 private:
  std::string& s_;
};

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t reserve = 2;  // [ ]
  for (std::size_t i = 0; i < n; ++i) {
    reserve += 1;  // {
    reserve += 5 + 10;  // "id": + uint32
    reserve += 8 + 5;   // ,"flag": + false/true max
    reserve += 9 + 10;  // ,"value": + uint32
    reserve += 8;       // ,"name":"
    reserve += names[i].size() + 8;  // escaped content estimate slack
    reserve += 2;       // "}
    if (i + 1 != n) reserve += 1;  // ,
  }

  std::string out;
  out.reserve(reserve);

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
