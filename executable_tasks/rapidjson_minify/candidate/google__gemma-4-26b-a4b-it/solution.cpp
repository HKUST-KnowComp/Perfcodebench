#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

std::string minify_json(const std::string& json_input) {
    rapidjson::Document d;
    // Parse in-situ is not possible here as we don't own the input string buffer in a way that allows modification,
    // but standard Parse is still much faster than nlohmann.
    if (d.Parse(json_input.c_str()).HasParseError()) {
        return "";
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);

    return buffer.GetString();
}