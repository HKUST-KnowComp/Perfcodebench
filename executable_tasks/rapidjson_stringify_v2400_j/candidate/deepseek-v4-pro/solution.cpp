#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names)
{
    rapidjson::StringBuffer buffer;
    if (!ids.empty()) {
        // pre-allocate a conservative estimate to avoid multiple reallocations
        buffer.Reserve(ids.size() * 80 + 2);
    }

    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.SetMaxDecimalPlaces(0); // ensure integers stay integer-like

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
        writer.String(names[i].c_str(), static_cast<rapidjson::SizeType>(names[i].size()), true);

        writer.EndObject();
    }
    writer.EndArray();

    return std::string(buffer.GetString(), buffer.GetSize());
}