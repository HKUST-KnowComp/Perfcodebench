#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <string>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  // Calculate total string length for pre-allocation estimate
  size_t total_name_len = 0;
  for (const auto& name : names) {
    total_name_len += name.size();
  }
  // Rough estimate per record: fixed overhead ~50 bytes + up to 20 digits
  // Add extra margin to avoid reallocation
  size_t est_size = ids.size() * 70 + total_name_len;
  rapidjson::StringBuffer sb;
  sb.Reserve(est_size);

  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

  writer.StartArray();
  for (size_t i = 0; i < ids.size(); ++i) {
    writer.StartObject();
    writer.Key("id");
    writer.Uint64(static_cast<uint64_t>(ids[i]));
    writer.Key("flag");
    writer.Bool((ids[i] % 3U) == 0U);
    writer.Key("value");
    writer.Uint64(static_cast<uint64_t>(values[i]));
    writer.Key("name");
    writer.String(names[i].c_str(), names[i].size());
    writer.EndObject();
  }
  writer.EndArray();

  return std::string(sb.GetString(), sb.GetSize());
}