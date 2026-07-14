#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

std::string minify_json(const std::string& json_input) {
    thread_local rapidjson::Document doc;
    thread_local rapidjson::StringBuffer buffer;
    buffer.Clear();
    doc.Parse<rapidjson::kParseDefaultFlags>(json_input.c_str());
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return std::string(buffer.GetString(), buffer.GetSize());
}