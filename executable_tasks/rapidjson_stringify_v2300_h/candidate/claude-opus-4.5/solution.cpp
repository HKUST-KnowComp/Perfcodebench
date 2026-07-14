#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  
  rapidjson::StringBuffer buffer;
  // Estimate: each record ~50 bytes average, reserve to avoid reallocations
  buffer.Reserve(ids.size() * 64 + 2);
  
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  
  writer.StartArray();
  
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    writer.StartObject();
    
    // "id": <uint32>
    writer.Key("id", 2);
    writer.Uint(ids[i]);
    
    // "flag": <bool> - true if id % 3 == 0
    writer.Key("flag", 4);
    writer.Bool((ids[i] % 3U) == 0U);
    
    // "value": <uint32>
    writer.Key("value", 5);
    writer.Uint(values[i]);
    
    // "name": <string>
    writer.Key("name", 4);
    const std::string& name = names[i];
    writer.String(name.c_str(), static_cast<rapidjson::SizeType>(name.size()));
    
    writer.EndObject();
  }
  
  writer.EndArray();
  
  return std::string(buffer.GetString(), buffer.GetSize());
}
