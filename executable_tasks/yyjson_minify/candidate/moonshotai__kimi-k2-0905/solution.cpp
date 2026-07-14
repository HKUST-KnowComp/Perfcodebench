#include "interface.h"
#include <yyjson.h>
#include <string>

std::string minify_json(const std::string& json_input) {
    // Read-only flag set: no string copies inside yyjson
    yyjson_read_flag flg = YYJSON_READ_NOFLAG;
    yyjson_read_err err;
    yyjson_doc* doc = yyjson_read_opts(const_cast<char*>(json_input.data()),
                                        json_input.size(),
                                        flg,
                                        nullptr, &err);
    if (!doc) {
        // Should never happen for well-formed test data; fall back gracefully
        yyjson_doc_free(doc);
        return json_input; // invalid JSON untouched
    }

    // Compute required output length (minified)
    size_t len = yyjson_write(doc, YYJSON_WRITE_NOFLAG, nullptr);
    if (len == 0) {
        yyjson_doc_free(doc);
        return json_input;
    }

    std::string out;
    out.resize_and_overwrite(len, [&](char* buf, size_t n) {
        size_t written = yyjson_write_opts(doc,
                                           YYJSON_WRITE_NOFLAG,
                                           nullptr,
                                           buf, n);
        return written ? written : 0;
    });

    yyjson_doc_free(doc);
    return out;
}