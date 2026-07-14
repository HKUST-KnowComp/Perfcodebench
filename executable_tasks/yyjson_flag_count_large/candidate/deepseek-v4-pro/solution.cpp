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
    yyjson_val *item;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);
    while ((item = yyjson_arr_iter_next(&iter))) {
        yyjson_val *flag_val = yyjson_obj_get(item, "flag");
        if (flag_val && yyjson_is_true(flag_val)) {
            yyjson_val *id_val = yyjson_obj_get(item, "id");
            uint64_t id = id_val ? yyjson_get_uint(id_val) : 0;
            yyjson_val *value_val = yyjson_obj_get(item, "value");
            uint64_t value = value_val ? yyjson_get_uint(value_val) : 0;
            yyjson_val *name_val = yyjson_obj_get(item, "name");
            uint64_t name_len = 0;
            if (name_val && yyjson_is_str(name_val)) {
                name_len = yyjson_get_len(name_val);
            }
            sum += id;
            sum += value;
            sum += name_len;
        }
    }
    yyjson_doc_free(doc);
    return sum;
}
