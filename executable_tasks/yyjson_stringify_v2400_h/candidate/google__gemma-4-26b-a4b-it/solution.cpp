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
    // This avoids building an intermediate DOM tree.
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

    // Retrieve the serialized string
    const char *json_str = yyjson_writer_as_string(writer);
    std::string result;
    if (json_str) {
        result.assign(json_str);
    }

    yyjson_writer_free(writer);
    return result;
}