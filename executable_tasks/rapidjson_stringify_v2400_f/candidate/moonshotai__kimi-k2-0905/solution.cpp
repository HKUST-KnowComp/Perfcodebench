#include "interface.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <cstdint>
#include <string>
#include <vector>

std::string build_json(const std::vector<uint32_t>& ids,
                       const std::vector<uint32_t>& values,
                       const std::vector<std::string>& names)
{
    if (ids.empty()) return "[]";

    rapidjson::StringBuffer sb;
    sb.Reserve(64U * ids.size());

    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    w.StartArray();

    const bool* __restrict flag_ptr = nullptr; // avoid branch in loop

    for (size_t i = 0, n = ids.size(); i < n; ++i)
    {
        w.StartObject();

        w.Key("id");
        w.Uint(ids[i]);

        w.Key("flag");
        w.Bool((ids[i] % 3U) == 0U);

        w.Key("value");
        w.Uint(values[i]);

        w.Key("name");
        w.String(names[i].data(), names[i].size());

        w.EndObject();
    }

    w.EndArray();
    return std::string(sb.GetString(), sb.GetSize());
}