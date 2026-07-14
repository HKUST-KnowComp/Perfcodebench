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
    const size_t n = ids.size();
    rapidjson::StringBuffer buffer;
    buffer.Reserve(32 + n * 64);          // rough upper bound to avoid realloc
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

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