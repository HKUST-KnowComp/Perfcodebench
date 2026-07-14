#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    const size_t n = ids.size();
    // Empirical reserve: 50 chars per object is safe for typical inputs
    buffer.Reserve(n * 50 + 32);

    writer.StartArray();
    for (size_t i = 0; i < n; ++i) {
        writer.StartObject();

        writer.Key("id", 2);
        writer.Uint(ids[i]);

        writer.Key("flag", 4);
        writer.Bool((ids[i] % 3U) == 0U);

        writer.Key("value", 5);
        writer.Uint(values[i]);

        writer.Key("name", 4);
        writer.String(names[i].data(), names[i].size());

        writer.EndObject();
    }
    writer.EndArray();

    return std::string(buffer.GetString(), buffer.GetSize());
}