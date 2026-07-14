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
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    const size_t n = ids.size();
    w.StartArray();
    for (size_t i = 0; i < n; ++i) {
        w.StartObject();
        w.Key("id", 2);        w.Uint(ids[i]);
        w.Key("flag", 4);      w.Bool((ids[i] % 3U) == 0U);
        w.Key("value", 5);     w.Uint(values[i]);
        w.Key("name", 4);      w.String(names[i].data(), names[i].size());
        w.EndObject();
    }
    w.EndArray();
    return std::string(buf.GetString(), buf.GetSize());
}