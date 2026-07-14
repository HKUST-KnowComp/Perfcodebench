#include "interface.h"
#include <yyjson.h>
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) {
        return 0;
    }

    uint64_t sum = 0;
    yyjson_val *root = yyjson_doc_get_root(doc);

    if (yyjson_is_arr(root)) {
        yyjson_val *item;
        yyjson_iter iter;
        yyjson_iter_init(root, &iter);

        while ((item = yyjson_iter_next(&iter))) {
            if (!yyjson_is_obj(item)) continue;

            // Check flag
            yyjson_val *flag_val = yyjson_obj_get(item, "flag");
            if (!flag_val || !yyjson_is_bool(flag_val) || !yyjson_get_bool(flag_val)) {
                continue;
            }

            // Get name size
            yyjson_val *name_val = yyjson_obj_get(item, "name");
            uint64_t name_len = 0;
            if (name_val && yyjson_is_str(name_val)) {
                name_len = yyjson_get_str_len(name_val);
            }

            // Get id
            yyjson_val *id_val = yyjson_obj_get(item, "id");
            uint64_t id_val_num = 0;
            if (id_val && yyjson_is_uint(id_val)) {
                id_val_num = yyjson_get_uint(id_val);
            }

            // Get value
            yyjson_val *value_val = yyjson_obj_get(item, "value");
            uint64_t value_val_num = 0;
            if (value_val && yyjson_is_uint(value_val)) {
                value_val_num = yyjson_get_uint(value_val);
            }

            sum += id_val_num;
            sum += value_val_num;
            sum += name_len;
        }
    }

    yyjson_doc_free(doc);
    return sum;
}