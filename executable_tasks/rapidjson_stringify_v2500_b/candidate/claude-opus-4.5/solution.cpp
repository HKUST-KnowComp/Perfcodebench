#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace {

// Fast uint32 to string, returns pointer past last digit
inline char* u32toa(uint32_t v, char* buf) {
    char tmp[10];
    char* p = tmp + 10;
    do {
        *--p = '0' + (v % 10);
        v /= 10;
    } while (v);
    while (p < tmp + 10) *buf++ = *p++;
    return buf;
}

} // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    rapidjson::StringBuffer sb;
    // Estimate: each record ~50 bytes base + name length
    size_t est = 2 + ids.size() * 60;
    for (const auto& n : names) est += n.size();
    sb.Reserve(est);
    
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    
    w.StartArray();
    for (size_t i = 0, n = ids.size(); i < n; ++i) {
        w.StartObject();
        
        w.Key("id", 2);
        w.Uint(ids[i]);
        
        w.Key("flag", 4);
        w.Bool((ids[i] % 3U) == 0U);
        
        w.Key("value", 5);
        w.Uint(values[i]);
        
        w.Key("name", 4);
        const std::string& nm = names[i];
        w.String(nm.data(), static_cast<rapidjson::SizeType>(nm.size()));
        
        w.EndObject();
    }
    w.EndArray();
    
    return std::string(sb.GetString(), sb.GetSize());
}
