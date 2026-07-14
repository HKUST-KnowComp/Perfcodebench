#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <cstddef>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    rapidjson::StringBuffer buffer;
    // Estimate capacity to reduce reallocations
    std::size_t total_name_chars = 0;
    for (const auto& n : names) total_name_chars += n.size();
    buffer.Reserve(ids.size() * 60 + total_name_chars + 10);
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
        writer.String(names[i].c_str(), static_cast<rapidjson::SizeType>(names[i].length()));
        writer.EndObject();
    }
    writer.EndArray();
    return std::string(buffer.GetString(), buffer.GetSize());
}
