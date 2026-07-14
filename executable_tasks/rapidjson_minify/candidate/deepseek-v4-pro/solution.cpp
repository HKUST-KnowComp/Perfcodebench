#include "interface.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <stdexcept>

std::string minify_json(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());
    if (doc.HasParseError()) {
        throw std::runtime_error("RapidJSON parse error: " + std::to_string(doc.GetParseError()));
    }

    rapidjson::StringBuffer buffer;
    buffer.Reserve(json_input.size());  // Pre-allocate to reduce reallocations

    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    return std::string(buffer.GetString(), buffer.GetSize());
}
