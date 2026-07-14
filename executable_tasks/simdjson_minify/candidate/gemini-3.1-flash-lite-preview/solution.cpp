#include "interface.h"
#include "simdjson.h"
#include <string>

static void minify_element(const simdjson::dom::element& element, std::string& out) {
    switch (element.type()) {
        case simdjson::dom::element_type::OBJECT: {
            out += '{';
            bool first = true;
            for (auto field : element.get_object()) {
                if (!first) out += ',';
                out += '"';
                out += std::string_view(field.key);
                out += "":";
                minify_element(field.value, out);
                first = false;
            }
            out += '}';
            break;
        }
        case simdjson::dom::element_type::ARRAY: {
            out += '[';
            bool first = true;
            for (auto child : element.get_array()) {
                if (!first) out += ',';
                minify_element(child, out);
                first = false;
            }
            out += ']';
            break;
        }
        case simdjson::dom::element_type::STRING: {
            out += '"';
            out += std::string_view(element.get_string().value());
            out += '"';
            break;
        }
        case simdjson::dom::element_type::INT64:
            out += std::to_string(element.get_int64());
            break;
        case simdjson::dom::element_type::UINT64:
            out += std::to_string(element.get_uint64());
            break;
        case simdjson::dom::element_type::DOUBLE:
            out += std::to_string(element.get_double());
            break;
        case simdjson::dom::element_type::BOOL:
            out += (element.get_bool() ? "true" : "false");
            break;
        case simdjson::dom::element_type::NULL_VALUE:
            out += "null";
            break;
    }
}

std::string minify_json(const std::string& json_input) {
    simdjson::dom::parser parser;
    simdjson::dom::element doc = parser.parse(json_input);
    std::string result;
    result.reserve(json_input.size());
    minify_element(doc, result);
    return result;
}