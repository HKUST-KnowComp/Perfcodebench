#include "interface.h"
#include <yyjson.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    size_t len = yyjson_arr_len(root);
    for (size_t i = 0; i < len; ++i) {
        yyjson_val *item = yyjson_arr_get(root, i);
        if (!yyjson_is_obj(item)) continue;

        yyjson_val *flag_val = yyjson_obj_get(item, "flag");
        if (!yyjson_is_bool(flag_val) || !yyjson_get_bool(flag_val)) {
            continue;
        }

        yyjson_val *name_val = yyjson_obj_get(item, "name");
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        yyjson_val *value_val = yyjson_obj_get(item, "value");

        if (yyjson_is_str(name_val) && yyjson_is_uint(id_val) && yyjson_is_uint(value_val)) {
            sum += yyjson_get_uint(id_val);
            sum += yyjson_get_uint(value_val);
            sum += static_cast<uint64_t>(yyjson_str_len(name_val));
        }
    }

    yyjson_doc_free(doc);
    return sum;
}