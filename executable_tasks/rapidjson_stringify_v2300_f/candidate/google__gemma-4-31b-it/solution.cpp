#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    rapidjson::StringBuffer buffer;
    // Pre-allocate buffer to reduce reallocations based on a heuristic
    // Average object size: "{\"id\":10,\"flag\":true,\"value\":10,\"name\":\"name\"}," 
    // roughly 60-80 bytes per element.
    if (!ids.empty()) {
        buffer.Reserve(ids.size() * 70);
    }

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