#include "interface.h"
#include <cstddef>
#include <rapidjson/writer.h>

struct StringStream {
    std::string& str;
    explicit StringStream(std::string& s) : str(s) {}
    void Put(char c) { str += c; }
    void Flush() {}
};

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    std::size_t total_size = 0;
    for (const auto& n : names) {
        total_size += n.size();
    }
    total_size += ids.size() * 40; // estimate overhead per object
    std::string out;
    out.reserve(total_size);
    StringStream ss(out);
    rapidjson::Writer<StringStream> writer(ss);
    writer.StartArray();
    for (std::size_t i = 0; i < ids.size(); ++i) {
        writer.StartObject();
        writer.Key("id");
        writer.Uint(ids[i]);
        writer.Key("flag");
        writer.Bool(ids[i] % 3 == 0);
        writer.Key("value");
        writer.Uint(values[i]);
        writer.Key("name");
        writer.String(names[i].data(), static_cast<rapidjson::SizeType>(names[i].size()));
        writer.EndObject();
    }
    writer.EndArray();
    return out;
}