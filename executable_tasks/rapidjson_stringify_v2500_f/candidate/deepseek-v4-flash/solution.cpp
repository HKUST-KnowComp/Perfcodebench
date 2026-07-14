#include "interface.h"
#include "rapidjson/writer.h"
#include <string>
#include <vector>

struct StringBuffer {
    std::string str;
    void Put(char c) { str += c; }
    void Flush() {}
};

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    StringBuffer sb;
    sb.str.reserve(ids.size() * 64);
    rapidjson::Writer<StringBuffer> writer(sb);
    writer.StartArray();
    for (std::size_t i = 0; i < ids.size(); ++i) {
        writer.StartObject();
        writer.Key("id");
        writer.Uint(ids[i]);
        writer.Key("flag");
        writer.Bool(ids[i] % 3U == 0U);
        writer.Key("value");
        writer.Uint(values[i]);
        writer.Key("name");
        writer.String(names[i].c_str(), names[i].size());
        writer.EndObject();
    }
    writer.EndArray();
    return std::move(sb.str);
}
