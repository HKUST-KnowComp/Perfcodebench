#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <cstdint>
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    size_t n = ids.size();
    if (n == 0) {
        return "[]";
    }

    // Heuristic for pre-allocation: 
    // ~50 bytes for keys and structure + ~10 bytes per uint32 + name length
    // This reduces reallocations during the stringification process.
    rapidjson::StringBuffer sb;
    size_t estimated_size = n * 64;
    for (const auto& name : names) {
        estimated_size += name.length();
    }
    sb.Reserve(estimated_size);

    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    writer.StartArray();
    for (size_t i = 0; i < n; ++i) {
        writer.StartObject();
        
        // Field: id
        writer.Key("id", 2);
        writer.Uint(ids[i]);
        
        // Field: flag
        writer.Key("flag", 4);
        writer.Bool((ids[i] % 3U) == 0U);
        
        // Field: value
        writer.Key("value", 5);
        writer.Uint(values[i]);
        
        // Field: name
        writer.Key("name", 4);
        writer.String(names[i].c_str(), static_cast<rapidjson::SizeType>(names[i].length()));
        
        writer.EndObject();
    }
    writer.EndArray();

    return std::string(sb.GetString(), sb.GetSize());
}