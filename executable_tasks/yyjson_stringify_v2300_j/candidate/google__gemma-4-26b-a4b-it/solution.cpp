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

    // Use yyjson_writer for high-performance, single-pass serialization.
    // This avoids the overhead of building a full DOM tree in memory.
    yyjson_writer *writer = yyjson_writer_new(NULL);
    if (!writer) return "[]";

    yyjson_write_start_array(writer);
    for (size_t i = 0; i < n; ++i) {
        yyjson_write_start_object(writer);
        
        // Field: "id"
        yyjson_write_key(writer, "id");
        yyjson_write_uint(writer, ids[i]);

        // Field: "flag"
        yyjson_write_key(writer, "flag");
        yyjson_write_bool(writer, (ids[i] % 3U) == 0U);

        // Field: "value"
        yyjson_write_key(writer, "value");
        yyjson_write_uint(writer, values[i]);

        // Field: "name"
        yyjson_write_key(writer, "name");
        yyjson_write_str(writer, names[i].c_str());

        yyjson_write_end_object(writer);
    }
    yyjson_write_end_array(writer);

    // Retrieve the serialized string.
    // yyjson_writer_write returns the buffer.
    const char *res = yyjson_writer_write(writer);
    size_t len = yyjson_writer_len(writer);

    std::string result;
    if (res && len > 0) {
        result.assign(res, len);
    }

    yyjson_writer_free(writer);
    return result;
}