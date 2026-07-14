#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringwriter.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    // Heuristic for initial capacity to reduce reallocations
    // Average object size: "{\"id\":10,\"flag\":true,\"value\":10,\"name\":\"name\"}," 
    // Roughly 60-100 bytes per element
    std::string result;
    result.reserve(ids.size() * 80);

    rapidjson::StringBuffer buffer;
    // Pre-reserve buffer capacity to avoid internal reallocations
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