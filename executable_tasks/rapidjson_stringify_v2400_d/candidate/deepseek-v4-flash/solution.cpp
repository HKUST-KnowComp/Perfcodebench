#include "interface.h"
#include <rapidjson/writer.h>
#include <string>

class RapidJSONStringStream {
public:
    explicit RapidJSONStringStream(std::string& s) : str_(s) {}
    void Put(char c) { str_.push_back(c); }
    void Flush() {}
private:
    std::string& str_;
};

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    std::string result;
    size_t estimate = 2;
    for (size_t i = 0; i < ids.size(); ++i) {
        estimate += 30 + names[i].size() * 1.1;
    }
    result.reserve(estimate);
    RapidJSONStringStream stream(result);
    rapidjson::Writer<RapidJSONStringStream> writer(stream);
    
    writer.StartArray();
    for (size_t i = 0; i < ids.size(); ++i) {
        writer.StartObject();
        writer.Key("id");
        writer.Uint(ids[i]);
        writer.Key("flag");
        writer.Bool(ids[i] % 3 == 0);
        writer.Key("value");
        writer.Uint(values[i]);
        writer.Key("name");
        writer.String(names[i].c_str(), names[i].size());
        writer.EndObject();
    }
    writer.EndArray();
    
    return result;
}