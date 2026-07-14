#include "interface.h"

#include <rapidjson/writer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  writer.StartArray();
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();
    writer.Key("id");
    writer.Uint(ids[i]);
    writer.Key("flag");
    writer.Bool((ids[i] % 3U) == 0U);
    writer.Key("value");
    writer.Uint(values[i]);
    writer.Key("name");
    writer.String(names[i].data(), names[i].size());
    writer.EndObject();
  }
  writer.EndArray();
  return std::string(sb.GetString(), sb.GetSize());
}