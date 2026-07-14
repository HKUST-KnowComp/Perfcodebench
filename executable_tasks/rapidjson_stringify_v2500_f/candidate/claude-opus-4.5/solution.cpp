#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  rapidjson::StringBuffer buffer;
  // Reserve approximate capacity to reduce reallocations
  // Each record: {"id":N,"flag":true/false,"value":N,"name":"..."}
  // Rough estimate: ~60 bytes per record + name length
  size_t n = ids.size();
  size_t estimated = 2 + n * 60;
  for (size_t i = 0; i < n; ++i) {
    estimated += names[i].size();
  }
  buffer.Reserve(estimated);
  
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  
  writer.StartArray();
  for (size_t i = 0; i < n; ++i) {
    writer.StartObject();
    
    writer.Key("id", 2);
    writer.Uint(ids[i]);
    
    writer.Key("flag", 4);
    writer.Bool((ids[i] % 3U) == 0U);
    
    writer.Key("value", 5);
    writer.Uint(values[i]);
    
    writer.Key("name", 4);
    writer.String(names[i].c_str(), static_cast<rapidjson::SizeType>(names[i].size()));
    
    writer.EndObject();
  }
  writer.EndArray();
  
  return std::string(buffer.GetString(), buffer.GetSize());
}