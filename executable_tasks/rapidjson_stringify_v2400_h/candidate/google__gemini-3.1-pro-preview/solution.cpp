#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <cstddef>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  
  size_t n = ids.size();
  if (n == 0) {
    return "[]";
  }

  // Heuristic for pre-allocation to reduce reallocations:
  // Each object: {"id":1234567890,"flag":false,"value":1234567890,"name":"..."}
  // Fixed overhead per object is ~50-60 bytes + name length.
  size_t estimated_size = 2; // []
  for (const auto& name : names) {
    estimated_size += 60 + name.length();
  }

  rapidjson::StringBuffer sb;
  sb.Reserve(estimated_size);
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

  writer.StartArray();
  for (size_t i = 0; i < n; ++i) {
    writer.StartObject();
    
    // Field 1: id
    writer.Key("id", 2);
    writer.Uint(ids[i]);
    
    // Field 2: flag
    writer.Key("flag", 4);
    writer.Bool((ids[i] % 3U) == 0U);
    
    // Field 3: value
    writer.Key("value", 5);
    writer.Uint(values[i]);
    
    // Field 4: name
    writer.Key("name", 4);
    writer.String(names[i].data(), static_cast<rapidjson::SizeType>(names[i].length()));
    
    writer.EndObject();
  }
  writer.EndArray();

  return std::string(sb.GetString(), sb.GetSize());
}