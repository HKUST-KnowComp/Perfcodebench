#include "interface.h"
#include <yyjson.h>
#include <stdexcept>
#include <cstdlib>

std::string minify_json(const std::string& json_input) {
    // Parse the JSON input preserving key order
    yyjson_read_err err;
    yyjson_doc *doc = yyjson_read_opts(json_input.c_str(), json_input.size(), 0, NULL, &err);
    if (!doc) {
        throw std::runtime_error("Failed to parse JSON: " +
            (err.msg ? std::string(err.msg) : "unknown error"));
    }

    // Generate minified JSON
    size_t write_len;
    char *json_str = yyjson_write(doc, 0, &write_len);
    yyjson_doc_free(doc);
    if (!json_str) {
        throw std::runtime_error("Failed to write minified JSON");
    }

    std::string result(json_str, write_len);
    free(json_str);  // yyjson strings are allocated with malloc
    return result;
}