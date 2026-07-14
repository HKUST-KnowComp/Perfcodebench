#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    size_t n = ids.size();
    if (n == 0) return "[]";

    // Heuristic for initial capacity to reduce reallocations
    // Each object: "{\"id\":1234567890,\"flag\":true,\"value\":1234567890,\"name\":\"some_name\"},"
    // Approx 60-100 bytes per element
    rapidjson::StringBuffer buffer;
    buffer.Reserve(n * 80);

    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    writer.StartArray();
    for (size_t i = 0; i < n; ++i) {
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