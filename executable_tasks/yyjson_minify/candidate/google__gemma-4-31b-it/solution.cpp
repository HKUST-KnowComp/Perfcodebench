#include "interface.h"
#include <yyjson.h>
#include <vector>

std::string minify_json(const std::string& json_input) {
    if (json_input.empty()) return "";

    // Parse the JSON input
    yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) {
        return "";
    }

    // Get the root value
    yyjson_val *root = yyjson_doc_get_root(doc);

    // Write the JSON to a buffer in minified format
    // yyjson_write_opts is 0 for minified output
    yyjson_mut_buf buf = yyjson_mut_buf_init();
    yyjson_write_opts opts = 0;
    yyjson_write_opts_set_indent(opts, 0);

    // Use the write function to serialize the root value
    yyjson_write_buf(buf, doc, root, opts);

    // Extract the result from the buffer
    std::string result(buf.ptr, buf.len);

    // Cleanup
    yyjson_mut_buf_free(buf);
    yyjson_doc_free(doc);

    return result;
}