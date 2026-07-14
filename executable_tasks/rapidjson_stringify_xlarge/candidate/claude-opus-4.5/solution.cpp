#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  
  rapidjson::StringBuffer buffer;
  // Pre-reserve capacity: estimate ~50 bytes per record + name lengths
  size_t estimated_size = 64 * ids.size();
  for (const auto& name : names) {
    estimated_size += name.size();
  }
  buffer.Reserve(estimated_size);
  
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  
  writer.StartArray();
  
  for (std::size_t i = 0; i < ids.size(); ++i) {
    writer.StartObject();
    
    writer.Key("id", 2);
    writer.Uint(ids[i]);
    
    writer.Key("flag", 4);
    writer.Bool((ids[i] % 3U) == 0U);
    
    writer.Key("value", 5);
    writer.Uint(values[i]);
    
    writer.Key("name", 4);
    const std::string& name = names[i];
    writer.String(name.c_str(), static_cast<rapidjson::SizeType>(name.size()));
    
    writer.EndObject();
  }
  
  writer.EndArray();
  
  return std::string(buffer.GetString(), buffer.GetSize());
}