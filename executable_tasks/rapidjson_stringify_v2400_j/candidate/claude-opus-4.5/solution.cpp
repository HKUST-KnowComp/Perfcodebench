#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();
  
  // Estimate buffer size: each record ~60 bytes average, plus array brackets
  rapidjson::StringBuffer buffer;
  buffer.Reserve(n * 64 + 16);
  
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