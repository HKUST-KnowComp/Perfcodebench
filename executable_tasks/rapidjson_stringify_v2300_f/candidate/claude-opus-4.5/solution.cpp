#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  
  const std::size_t n = ids.size();
  
  // Estimate output size to minimize reallocations
  // Each object: {"id":X,"flag":true/false,"value":Y,"name":"..."}
  // Rough estimate: 50 bytes base + name length per object
  std::size_t estimated_size = 2; // []
  for (std::size_t i = 0; i < n; ++i) {
    estimated_size += 60 + names[i].size();
  }
  
  rapidjson::StringBuffer buffer;
  buffer.Reserve(estimated_size);
  
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  
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
    const std::string& name = names[i];
    writer.String(name.data(), static_cast<rapidjson::SizeType>(name.size()));
    
    writer.EndObject();
  }
  
  writer.EndArray();
  
  return std::string(buffer.GetString(), buffer.GetSize());
}
