#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <vector>
#include <string>
#include <cstdint>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    const size_t n = ids.size();
    if (n == 0) {
        return "[]";
    }

    // Heuristic for pre-allocation: 
    // Each object: {"id":10,"flag":false,"value":10,"name":""} is ~50 bytes + name length.
    // Pre-reserving helps avoid multiple reallocations in the StringBuffer.
    rapidjson::StringBuffer sb;
    sb.Reserve(n * 64);

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
        const std::string& name = names[i];
        writer.String(name.c_str(), static_cast<rapidjson::SizeType>(name.length()));
        
        writer.EndObject();
    }
    writer.EndArray();

    return std::string(sb.GetString(), sb.GetSize());
}