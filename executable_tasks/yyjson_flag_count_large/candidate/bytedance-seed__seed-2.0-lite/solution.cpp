#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0);
    yyjson_val* root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;
    const size_t num_items = yyjson_arr_size(root);
    for (size_t i = 0; i < num_items; ++i) {
        yyjson_val* item = yyjson_arr_get(root, i);
        yyjson_val* flag = yyjson_obj_get(item, "flag");
        if (!yyjson_get_bool(flag)) continue;
        sum += yyjson_get_uint64(yyjson_obj_get(item, "id"));
        sum += yyjson_get_uint64(yyjson_obj_get(item, "value"));
        yyjson_val* name = yyjson_obj_get(item, "name");
        sum += static_cast<uint64_t>(yyjson_get_len(name));
    }
    yyjson_doc_free(doc);
    return sum;
}