#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string>
#include <cstdio>

std::string build_json(const std::vector<uint32_t>& ids,
                       const std::vector<uint32_t>& values,
                       const std::vector<std::string>& names)
{
    const size_t n = ids.size();
    // Upper-bound capacity: [{\"id\":4294967295,\"flag\":false,\"value\":4294967295,\"name\":\"...\"},] × n
    size_t est = 1 + n * (50 + 64) + 1;
    std::string out;
    out.reserve(est);

    rapidjson::StringBuffer buf;
    buf.Clear();
    buf.Push(out.data(), out.capacity());
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    w.StartArray();
    for (size_t i = 0; i < n; ++i) {
        w.StartObject();
        w.Key("id", 2);     w.Uint(ids[i]);
        w.Key("flag", 4);   w.Bool((ids[i] % 3u) == 0u);
        w.Key("value", 5);  w.Uint(values[i]);
        w.Key("name", 4);   w.String(names[i].data(), names[i].size());
        w.EndObject();
    }
    w.EndArray();

    out.assign(buf.GetString(), buf.GetSize());
    return out;
}