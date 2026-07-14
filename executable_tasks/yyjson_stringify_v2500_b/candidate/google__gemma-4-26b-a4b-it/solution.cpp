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

    size_t len;
    const char *res = yyjson_writer_as_string(writer, &len);
    
    std::string output;
    if (res) {
        output.assign(res, len);
    }

    yyjson_writer_free(writer);
    return output;
}