#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string>
#include <vector>
#include <cstdint>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const size_t n = ids.size();

  // Empirical size guess: 30 bytes per element is safe for minified JSON.
  rapidjson::StringBuffer buf;
  buf.Reserve(n * 30);

  rapidjson::Writer<rapidjson::StringBuffer> w(buf);
  w.StartArray();
  for (size_t i = 0; i < n; ++i) {
    w.StartObject();

    w.Key("id", 2);
    w.Uint(ids[i]);

    w.Key("flag", 4);
    w.Bool((ids[i] % 3U) == 0U);

    w.Key("value", 5);
    w.Uint(values[i]);

    const std::string& name = names[i];
    w.Key("name", 4);
    w.String(name.data(), name.size());

    w.EndObject();
  }
  w.EndArray();

  return std::string(buf.GetString(), buf.GetSize());
}