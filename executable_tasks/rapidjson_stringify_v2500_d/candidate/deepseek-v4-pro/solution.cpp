#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    using namespace rapidjson;
    StringBuffer s;

    // Rough upper-bound estimate to avoid reallocations
    size_t totalLen = 2; // brackets
    for (size_t i = 0; i < ids.size(); ++i) {
        totalLen += 1; // '{'
        totalLen += 4 + 1; // "id":
        totalLen += 10;    // max digits for uint32_t
        totalLen += 1; // ','
        totalLen += 6 + 1; // "flag":
        totalLen += 5;    // max "false"
        totalLen += 1; // ','
        totalLen += 7 + 1; // "value":
        totalLen += 10;    // max digits for uint32_t
        totalLen += 1; // ','
        totalLen += 6 + 1; // "name":
        totalLen += names[i].size() + 2; // quotes, worst-case no escape expansion
        totalLen += 1; // '}'
    }
    if (!ids.empty()) totalLen += ids.size() - 1; // commas between objects
    s.Reserve(totalLen + 1024); // added safety margin

    Writer<StringBuffer> writer(s);
    writer.StartArray();
    for (size_t i = 0; i < ids.size(); ++i) {
        writer.StartObject();
        writer.Key("id");
        writer.Uint(ids[i]);
        writer.Key("flag");
        writer.Bool((ids[i] % 3U) == 0U);
        writer.Key("value");
        writer.Uint(values[i]);
        writer.Key("name");
        writer.String(names[i].c_str(), static_cast<SizeType>(names[i].size()));
        writer.EndObject();
    }
    writer.EndArray();
    return std::string(s.GetString(), s.GetSize());
}