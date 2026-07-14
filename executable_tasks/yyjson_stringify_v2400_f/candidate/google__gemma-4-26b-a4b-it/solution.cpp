#include "interface.h"
#include <yyjson.h>
#include <vector>
#include <string>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    size_t n = ids.size();
    if (n == 0) return "[]";

    // Use yyjson_writer for high-performance, low-allocation stringification.
    // This avoids building a full DOM tree in memory and goes straight to a buffer.
    yyjson_writer *writer = yyjson_writer_new(NULL);
    if (!writer) return "[]";

    yyjson_write_start_array(writer);
    for (size_t i = 0; i < n; ++i) {
        yyjson_write_start_object(writer);
        
        // Field: id
        yyjson_write_key(writer, "id");
        yyjson_write_uint(writer, ids[i]);

        // Field: flag
        yyjson_write_key(writer, "flag");
        yyjson_write_bool(writer, (ids[i] % 3U) == 0U);

        // Field: value
        yyjson_write_key(writer, "value");
        yyjson_write_uint(writer, values[i]);

        // Field: name
        yyjson_write_key(writer, "name");
        yyjson_write_str(writer, names[i].c_str());

        yyjson_write_end_object(writer);
    }
    yyjson_write_end_array(writer);

    // Retrieve the resulting string
    const char *res = yyjson_writer_buffer(writer);
    size_t len = yyjson_writer_len(writer);
    
    std::string result(res, len);

    yyjson_writer_free(writer);
    return result;
}