#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

std::string minify_json(const std::string& json_input) {
    if (json_input.empty()) return "";

    rapidjson::Document doc;
    // Parse the input string. RapidJSON preserves the order of members in objects.
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return "";
    }

    rapidjson::StringBuffer buffer;
    // Writer produces minified output by default (no indentation).
    rapidjson::Writer<rapidjson::GenericWriter<rapidjson::StringBuffer>> writer(buffer);
    doc.Accept(writer);

    return std::string(buffer.GetString(), buffer.GetSize());
}