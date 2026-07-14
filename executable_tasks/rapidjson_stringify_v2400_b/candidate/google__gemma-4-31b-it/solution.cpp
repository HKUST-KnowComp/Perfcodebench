#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringwriter.h>
#include <vector>
#include <string>

// Custom stream for RapidJSON to write directly into a std::string
struct StringStream {
    std::string* target;
    typedef char CharType;
    
    void Put(CharType c) {
        target->push_back(c);
    }
    
    void Put(const CharType* str, std::size_t len) {
        target->append(str, len);
    }
};

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    std::string result;
    // Heuristic for initial capacity to reduce reallocations
    // Average object: {"id":10, "flag":true, "value":10, "name":"..."} 
    // Approx 50-100 bytes per element
    result.reserve(ids.size() * 64);

    StringStream ss;
    ss.target = &result;
    rapidjson::Writer<StringStream> writer(ss);

    writer.StartArray();
    for (std::size_t i = 0; i < ids.size(); ++i) {
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