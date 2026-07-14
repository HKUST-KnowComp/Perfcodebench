#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();
  
  rapidjson::StringBuffer buffer;
  // Estimate: each record ~50-100 bytes, reserve generously
  buffer.Reserve(n * 80 + 16);
  
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  
  writer.StartArray();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();
    
    // "id": <uint>
    writer.Key("id", 2);
    writer.Uint(ids[i]);
    
    // "flag": <bool>
    writer.Key("flag", 4);
    writer.Bool((ids[i] % 3U) == 0U);
    
    // "value": <uint>
    writer.Key("value", 5);
    writer.Uint(values[i]);
    
    // "name": <string>
    writer.Key("name", 4);
    writer.String(names[i].data(), static_cast<rapidjson::SizeType>(names[i].size()));
    
    writer.EndObject();
  }
  writer.EndArray();
  
  return std::string(buffer.GetString(), buffer.GetSize());
}