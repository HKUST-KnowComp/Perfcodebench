#include "interface.h"
#include "yyjson.h"
#include <string>
#include <vector>

std::string build_json(const std::vector<uint32_t>& ids, const std::vector<uint32_t>& values, const std::vector<std::string>& names) {
    yyjson_writer* const writer = yyjson_writer_init(nullptr, 0);
    if (!writer) {
        return {};
    }

    yyjson_write_array_start(writer);
    const size_t count = ids.size();
    for (size_t i = 0; i < count; ++i) {
        yyjson_write_start_object(writer);

        yyjson_write_key_val_u32(writer, "id", ids[i]);
        yyjson_write_key_val_bool(writer, "flag", (ids[i] % 3U) == 0U);
        yyjson_write_key_val_u32(writer, "value", values[i]);
        yyjson_write_key_val_string_len(writer, "name", names[i].data(), names[i].size());

        yyjson_write_end(writer);
    }
    yyjson_write_array_end(writer);

    size_t json_len = 0;
    const char* const json_data = yyjson_write_get_data(writer, &json_len);
    std::string result(json_data, json_len);

    yyjson_writer_free(writer);
    return result;
}