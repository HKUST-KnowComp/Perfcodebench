#include "interface.h"
#include "yyjson.h"
#include <string>
#include <vector>

std::string minify_json(const std::string& json_input) {
    // Use yyjson to read the input string
    yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) return "";

    // Write the document to a buffer in minified format (no indentation)
    size_t len = 0;
    char *minified = yyjson_write(doc->root, YYJSON_WRITE_NOFLAG, &len);

    std::string result;
    if (minified) {
        result.assign(minified, len);
        free(minified);
    }

    yyjson_doc_free(doc);
    return result;
}