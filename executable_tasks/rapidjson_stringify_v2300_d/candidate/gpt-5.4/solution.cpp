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

 private:
  std::string& out_;
};

static inline std::size_t CountEscapedJsonStringSize(const std::string& s) {
  std::size_t extra = 0;
  for (unsigned char c : s) {
    switch (c) {
      case '"':
      case '\\':
      case '\b':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
        ++extra;
        break;
      default:
        if (c < 0x20) {
          extra += 5;  // "\\u00XX" instead of one byte
        }
        break;
    }
  }
  return s.size() + extra;
}

static inline std::size_t EstimateOutputSize(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();
  if (n == 0) return 2;  // []

  std::size_t total = 2;  // [ ]
  total += (n - 1);       // commas between objects

  constexpr std::size_t kFixedPerObject =
      29;  // {"id":,"flag":,"value":,"name":""}
  total += n * kFixedPerObject;

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t id = ids[i];
    uint32_t value = values[i];

    if (id >= 10U) {
      if (id >= 100U) {
        if (id >= 1000U) {
          if (id >= 10000U) {
            if (id >= 100000U) {
              if (id >= 1000000U) {
                if (id >= 10000000U) {
                  if (id >= 100000000U) {
                    if (id >= 1000000000U) total += 9;
                    else total += 8;
                  } else {
                    total += 7;
                  }
                } else {
                  total += 6;
                }
              } else {
                total += 5;
              }
            } else {
              total += 4;
            }
          } else {
            total += 3;
          }
        } else {
          total += 2;
        }
      } else {
        total += 1;
      }
    }

    if (value >= 10U) {
      if (value >= 100U) {
        if (value >= 1000U) {
          if (value >= 10000U) {
            if (value >= 100000U) {
              if (value >= 1000000U) {
                if (value >= 10000000U) {
                  if (value >= 100000000U) {
                    if (value >= 1000000000U) total += 9;
                    else total += 8;
                  } else {
                    total += 7;
                  }
                } else {
                  total += 6;
                }
              } else {
                total += 5;
              }
            } else {
              total += 4;
            }
          } else {
            total += 3;
          }
        } else {
          total += 2;
        }
      } else {
        total += 1;
      }
    }

    total += ((id % 3U) == 0U) ? 4 : 5;  // true / false
    total += CountEscapedJsonStringSize(names[i]);
  }

  return total;
}

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  std::string out;
  out.reserve(EstimateOutputSize(ids, values, names));

  StringBufferStream stream(out);
  rapidjson::Writer<StringBufferStream> writer(stream);

  writer.StartArray();
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t id = ids[i];
    writer.StartObject();

    writer.Key("id", 2);
    writer.Uint(id);

    writer.Key("flag", 4);
    writer.Bool((id % 3U) == 0U);

    writer.Key("value", 5);
    writer.Uint(values[i]);

    writer.Key("name", 4);
    const std::string& name = names[i];
    writer.String(name.data(), static_cast<rapidjson::SizeType>(name.size()));

    writer.EndObject();
  }
  writer.EndArray();

  return out;
}
