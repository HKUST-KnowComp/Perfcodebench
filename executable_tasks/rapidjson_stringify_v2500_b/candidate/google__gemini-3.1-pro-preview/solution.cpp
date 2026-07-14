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

    // Pre-allocate buffer to reduce reallocations.
    // Estimate: [ + n * ({"id":10,"flag":false,"value":10,"name":""} + name_len) + commas + ]
    // Fixed overhead per object is ~35 chars + keys. 
    // A safe heuristic: 64 bytes per entry + name lengths.
    size_t total_name_len = 0;
    for (const auto& name : names) {
        total_name_len += name.length();
    }
    
    rapidjson::StringBuffer sb(nullptr, 64 * n + total_name_len + 16);
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    writer.StartArray();
    for (size_t i = 0; i < n; ++i) {
        writer.StartObject();
        
        // Field 1: id
        writer.Key("id", 2);
        writer.Uint(ids[i]);
        
        // Field 2: flag
        writer.Key("flag", 4);
        writer.Bool((ids[i] % 3U) == 0U);
        
        // Field 3: value
        writer.Key("value", 5);
        writer.Uint(values[i]);
        
        // Field 4: name
        writer.Key("name", 4);
        writer.String(names[i].c_str(), static_cast<rapidjson::SizeType>(names[i].length()));
        
        writer.EndObject();
    }
    writer.EndArray();

    return std::string(sb.GetString(), sb.GetSize());
}