#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    rapidjson::StringBuffer sb;
    // Heuristic pre-allocation: ~60 bytes per record to reduce reallocations
    sb.Reserve(ids.size() * 60);
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

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
        writer.String(names[i].c_str(), static_cast<rapidjson::SizeType>(names[i].length()));
        
        writer.EndObject();
    }
    writer.EndArray();

    return std::string(sb.GetString(), sb.GetSize());
}