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

    w.StartArray();
    const size_t n = ids.size();
    for (size_t i = 0; i < n; ++i) {
        w.StartObject();
        w.Key("id");    w.Uint(ids[i]);
        w.Key("flag");  w.Bool((ids[i] % 3U) == 0U);
        w.Key("value"); w.Uint(values[i]);
        w.Key("name");  w.String(names[i].data(), names[i].size());
        w.EndObject();
    }
    w.EndArray();

    return std::string(buf.GetString(), buf.GetSize());
}