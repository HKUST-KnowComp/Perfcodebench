#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    // Pre-calculate an approximate size to reduce reallocations
    // Average object: "{"id":123,"flag":true,"value":123,"name":"name"},"
    // Roughly 60-100 bytes per entry
    rapidjson::StringBuffer buffer;
    buffer.Reserve(ids.size() * 80);
    
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    writer.StartArray();
    for (std::size_t i = 0; i < ids.size(); ++i) {
        writer.StartObject();
        
        writer.Key("id");
        writer.Uint(ids[i]);
        
        writer.Key("flag");
        writer.Bool((ids[i] % 3U) == 0U);
        
        writer.Key("value");
        writer.Uint(values[i]);
        
        writer.Key("name");
        writer.String(names[i].c_str());
        
        writer.EndObject();
    }
    writer.EndArray();
    
    return std::string(buffer.GetString(), buffer.GetSize());
}