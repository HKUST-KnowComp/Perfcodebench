#include "interface.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <cstring>

std::string minify_json(const std::string& json_input) {
    // Create a mutable copy for in-situ parsing
    std::vector<char> buffer(json_input.begin(), json_input.end());
    buffer.push_back('\0');
    
    rapidjson::Document doc;
    doc.ParseInsitu(buffer.data());
    
    rapidjson::StringBuffer sb;
    sb.Reserve(json_input.size()); // Pre-allocate to avoid reallocations
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    doc.Accept(writer);
    
    return std::string(sb.GetString(), sb.GetSize());
}