#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    size_t n = ids.size();
    yyjson_writer *writer = yyjson_writer_new(NULL);
    if (!writer) return "[]";

    yyjson_write_start_array(writer);
    for (size_t i = 0; i < n; ++i) {
        yyjson_write_start_object(writer);
        
        yyjson_write_key(writer, "id");
        yyjson_write_uint(writer, ids[i]);
        
        yyjson_write_key(writer, "flag");
        yyjson_write_bool(writer, (ids[i] % 3U) == 0U);
        
        yyjson_write_key(writer, "value");
        yyjson_write_uint(writer, values[i]);
        
        yyjson_write_key(writer, "name");
        yyjson_write_str(writer, names[i].c_str());
        
        yyjson_write_end_object(writer);
    }
    yyjson_write_end_array(writer);

    yyjson_stage *stage = yyjson_writer_stage(writer);
    yyjson_stage_write(stage);

    // Get the serialized string from the writer
    // yyjson_writer_get_buf returns the buffer used by the writer
    const char *buf = yyjson_writer_get_buf(writer);
    size_t len = yyjson_writer_get_len(writer);

    std::string result(buf, len);

    yyjson_writer_free(writer);
    return result;
}