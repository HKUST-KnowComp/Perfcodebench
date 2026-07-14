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

  // RapidJSON may use these in generic paths; provide minimal support.
  void PutN(char c, size_t n) { out_.append(n, c); }

 private:
  std::string& out_;
};

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t reserve = 2;  // [ ]
  for (std::size_t i = 0; i < n; ++i) {
    reserve += 40 + names[i].size();
  }

  std::string out;
  out.reserve(reserve);

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
