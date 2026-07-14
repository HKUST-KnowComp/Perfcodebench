#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    const size_t n = ids.size();
    if (n == 0) {
        return "[]";
    }

    // Heuristic for initial buffer capacity to minimize reallocations.
    // Each entry: {"id":1234567890,"flag":false,"value":1234567890,"name":"..."}
    // Approx 60-80 bytes per record + name length.
    rapidjson::StringBuffer sb;
    sb.Reserve(n * 80);

    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    writer.StartArray();
    for (size_t i = 0; i < n; ++i) {
        writer.StartObject();

        // Use Key(const Ch* str, SizeType length) to avoid strlen calls
        writer.Key("id", 2);
        writer.Uint(ids[i]);

        writer.Key("flag", 4);
        writer.Bool((ids[i] % 3U) == 0U);

        writer.Key("value", 5);
        writer.Uint(values[i]);

        writer.Key("name", 4);
        // Use String(const Ch* str, SizeType length) to avoid strlen calls
        writer.String(names[i].data(), static_cast<rapidjson::SizeType>(names[i].length()));

        writer.EndObject();
    }
    writer.EndArray();

    return std::string(sb.GetString(), sb.GetSize());
}