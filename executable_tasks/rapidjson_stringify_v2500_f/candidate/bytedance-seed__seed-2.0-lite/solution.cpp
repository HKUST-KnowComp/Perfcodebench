#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  rapidjson::StringBuffer buffer;
  // Pre-reserve buffer space to avoid expensive mid-serialization reallocations
  if (!ids.empty()) {
    buffer.Reserve(ids.size() * 128);
  }
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  writer.StartArray();
  for (size_t i = 0; i < ids.size(); ++i) {
    writer.StartObject();
    // Maintain exact field order to match baseline correctness requirements
    writer.Key("id");
    writer.Uint(ids[i]);
    writer.Key("flag");
    writer.Bool((ids[i] % 3U) == 0U);
    writer.Key("value");
    writer.Uint(values[i]);
    writer.Key("name");
    // Pass explicit string length to avoid redundant strlen calculation
    writer.String(names[i].data(), static_cast<rapidjson::SizeType>(names[i].size()));
    writer.EndObject();
  }
  writer.EndArray();

  return std::string(buffer.GetString(), buffer.GetSize());
}