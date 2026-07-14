#include "interface.h"
#include "yyjson.h"
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
    yyjson_val *item = root;
    while ((item = yyjson_arr_next(item))) {
        yyjson_val *flag_val = yyjson_obj_get(item, "flag");
        if (flag_val && yyjson_get_bool(flag_val)) {
            yyjson_val *name_val = yyjson_obj_get(item, "name");
            yyjson_val *id_val = yyjson_obj_get(item, "id");
            yyjson_val *value_val = yyjson_obj_get(item, "value");

            if (name_val) {
                const char *name_str = yyjson_get_str(name_val);
                if (name_str) {
                    sum += yyjson_get_len(name_val);
                }
            }
            if (id_val) {
                sum += (uint64_t)yyjson_get_int(id_val);
            }
            if (value_val) {
                sum += (uint64_t)yyjson_get_int(value_val);
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}