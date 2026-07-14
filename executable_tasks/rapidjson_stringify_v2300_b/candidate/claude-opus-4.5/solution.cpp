#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <cstring>
#include <charconv>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    const std::size_t n = ids.size();
    
    rapidjson::StringBuffer sb;
    // Reserve approximate capacity to avoid reallocations
    // Each record: {"id":X,"flag":true/false,"value":Y,"name":"..."}
    // Rough estimate: 60 bytes base + avg name length per record
    sb.Reserve(n * 80 + 2);
    
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    
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
    
    return std::string(sb.GetString(), sb.GetSize());
}