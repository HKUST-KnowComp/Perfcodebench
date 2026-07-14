#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringwriter.h>
#include <vector>
#include <string>

// Custom buffer for RapidJSON to write directly into a std::string
class StringWriterBuffer : public rapidjson::Writer::StringStream {
public:
    explicit StringWriterBuffer(std::string& s) : str(s) {}

    void Put(char c) override {
        str.push_back(c);
    }

    void Put(const char* str_ptr, size_t len) override {
        str.append(str_ptr, len);
    }

private:
    std::string& str;
};

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    std::string result;
    size_t n = ids.size();
    if (n == 0) return "[]";

    // Heuristic for initial capacity to reduce reallocations
    // "[{"id":10, "flag":true, "value":10, "name":"..."}, ... ]"
    // Approx 50-100 bytes per element
    result.reserve(n * 64);

    StringWriterBuffer buffer(result);
    rapidjson::Writer<StringWriterBuffer> writer(buffer);

    writer.StartArray();
    for (size_t i = 0; i < n; ++i) {
        writer.StartObject();
        
        writer.Key("id");
        writer.Uint(ids[i]);
        
        writer.Key("flag");
        writer.Bool((ids[i] % 3U) == 0U);
        
        writer.Key("value");
        writer.Uint(values[i]);
        
        writer.Key("name");
        writer.String(names[i].c_str());
        
        writer.EndObject();
    }
    writer.EndArray();

    return result;
}