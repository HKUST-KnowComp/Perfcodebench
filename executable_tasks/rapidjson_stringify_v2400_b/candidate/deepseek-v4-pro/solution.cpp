#include "interface.h"

#include <rapidjson/writer.h>

namespace {
struct StringStream {
    std::string& str;
    StringStream(std::string& str_) : str(str_) {}
    void Put(char c) { str.push_back(c); }
    void Flush() {}
    typedef char Ch;
};
}

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    std::string result;
    result.reserve(ids.size() * 50); // rough estimate to avoid reallocations
    StringStream ss(result);
    rapidjson::Writer<StringStream> writer(ss);
    writer.StartArray();
    for (std::size_t i = 0; i < ids.size(); ++i) {
        writer.StartObject();
        writer.Key("id");
        writer.Uint(ids[i]);
        writer.Key("flag");
        writer.Bool((ids[i] % 3) == 0);
        writer.Key("value");
        writer.Uint(values[i]);
        writer.Key("name");
        writer.String(names[i].c_str(), static_cast<rapidjson::SizeType>(names[i].size()), false);
        writer.EndObject();
    }
    writer.EndArray();
    return result;
}