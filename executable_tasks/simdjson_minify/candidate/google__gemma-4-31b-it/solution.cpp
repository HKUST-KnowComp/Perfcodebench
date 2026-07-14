#include "interface.h"
#include "simdjson.h"
#include <string>
#include <vector>

using namespace simdjson;

// Helper to recursively write minified JSON to a string
void minify_recursive(ondemand::document& doc, std::string& out) {
    auto type = doc.type();
    if (type == ondemand::json_type::array) {
        out += '[';
        bool first = true;
        for (auto element : doc) {
            if (!first) out += ',';
            minify_recursive(element, out);
            first = false;
        }
        out += ']';
    } else if (type == ondemand::json_type::object) {
        out += '{';
        bool first = true;
        for (auto field : doc.get_object()) {
            if (!first) out += ',';
            out += '"';
            out += std::string(field.key());
            out += '":';
            minify_recursive(field.value(), out);
            first = false;
        }
        out += '}';
    } else if (type == ondemand::json_type::string) {
        // Use raw_json to get the exact quoted string representation
        out += std::string(doc.raw_json());
    } else if (type == ondemand::json_type::boolean) {
        out += (doc.get_bool() ? "true" : "false");
    } else if (type == ondemand::json_type::number) {
        out += std::string(doc.raw_json());
    } else if (type == ondemand::json_type::null) {
        out += "null";
    }
}

std::string minify_json(const std::string& json_input) {
    if (json_input.empty()) return "";

    ondemand::parser parser;
    padded_string json = padded_string(json_input);
    ondemand::document doc = parser.iterate(json);

    std::string result;
    result.reserve(json_input.size());

    try {
        minify_recursive(doc, result);
    } catch (...) {
        // Fallback for malformed JSON if necessary, though contract implies valid JSON
        return "";
    }

    return result;
}