#include "interface.h"

#include <rapidjson/writer.h>
#include <string>
#include <vector>

namespace {

class StringBufferLike {
 public:
  using Ch = char;

  StringBufferLike() { s_.reserve(256); }

  void Put(char c) { s_.push_back(c); }
  void Flush() {}

  void PutN(char c, size_t n) { s_.append(n, c); }

  char* Push(size_t count) {
    const size_t old = s_.size();
    s_.resize(old + count);
    return &s_[old];
  }

  void Pop(size_t count) { s_.resize(s_.size() - count); }

  const std::string& GetString() const { return s_; }
  std::string&& MoveString() { return std::move(s_); }

  void Reserve(size_t n) { s_.reserve(n); }

 private:
  std::string s_;
};

static size_t estimate_size(const std::vector<uint32_t>& ids,
                            const std::vector<uint32_t>& values,
                            const std::vector<std::string>& names) {
  const size_t n = ids.size();
  size_t total = 2;  // [ ]
  for (size_t i = 0; i < n; ++i) {
    total += 37;  // punctuation + keys + booleans overhead excluding numbers/name content

    uint32_t x = ids[i];
    do {
      ++total;
      x /= 10;
    } while (x != 0);

    x = values[i];
    do {
      ++total;
      x /= 10;
    } while (x != 0);

    total += names[i].size() + 2;  // quotes; escaping may grow, this is a lower-bound-ish estimate
  }
  if (n > 1) total += n - 1;  // commas between objects
  return total;
}

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  StringBufferLike buffer;
  buffer.Reserve(estimate_size(ids, values, names));

  rapidjson::Writer<StringBufferLike> writer(buffer);
  writer.StartArray();

  const size_t n = ids.size();
  for (size_t i = 0; i < n; ++i) {
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
