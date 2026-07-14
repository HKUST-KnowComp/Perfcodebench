#include "interface.h"
#include <simdjson.h>
#include <string>

std::string minify_json(const std::string& json_input) {
    thread_local static simdjson::ondemand::parser parser;
    simdjson::ondemand::document doc;
    if (parser.iterate(json_input).get(doc) != simdjson::SUCCESS) {
        return ""; // invalid JSON -> empty string (same behaviour as baseline)
    }

    std::string out;
    out.reserve(json_input.size()); // minified is never larger than input

    auto write = [&](simdjson::ondemand::value v) -> bool {
        switch (v.type()) {
        case simdjson::ondemand::json_type::string: {
            std::string_view sv;
            if (v.get_string(sv) != simdjson::SUCCESS) return false;
            out.push_back('"');
            out.append(sv);
            out.push_back('"');
            return true;
        }
        case simdjson::ondemand::json_type::number: {
            std::string_view sv;
            if (v.get_raw_json_string(sv) != simdjson::SUCCESS) return false;
            out.append(sv);
            return true;
        }
        case simdjson::ondemand::json_type::boolean: {
            bool b;
            if (v.get_bool(b) != simdjson::SUCCESS) return false;
            out.append(b ? "true" : "false");
            return true;
        }
        case simdjson::ondemand::json_type::null:
            out.append("null");
            return true;
        case simdjson::ondemand::json_type::array: {
            out.push_back('[');
            simdjson::ondemand::array arr;
            if (v.get_array(arr) != simdjson::SUCCESS) return false;
            bool first = true;
            for (auto elem : arr) {
                if (!first) out.push_back(',');
                first = false;
                if (!write(elem.value())) return false;
            }
            out.push_back(']');
            return true;
        }
        case simdjson::ondemand::json_type::object: {
            out.push_back('{');
            simdjson::ondemand::object obj;
            if (v.get_object(obj) != simdjson::SUCCESS) return false;
            bool first = true;
            for (auto field : obj) {
                if (!first) out.push_back(',');
                first = false;
                std::string_view key;
                if (field.unescaped_key(key) != simdjson::SUCCESS) return false;
                out.push_back('"');
                out.append(key);
                out.push_back('"');
                out.push_back(':');
                if (!write(field.value())) return false;
            }
            out.push_back('}');
            return true;
        }
        default:
            return false;
        }
    };

    if (!write(doc)) return "";
    return out;
}