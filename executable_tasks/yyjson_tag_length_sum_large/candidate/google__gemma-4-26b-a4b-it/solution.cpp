#include "interface.h"
#include <yyjson.h>
#include <string>

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

            // Check 'flag' field
            yyjson_val *flag_val = yyjson_obj_get(yyjson_obj(item), "flag");
            if (!flag_val || !yyjson_is_bool(flag_val) || !yyjson_get_bool(flag_val)) {
                continue;
            }

            // Process 'tag' length
            yyjson_val *tag_val = yyjson_obj_get(yyjson_obj(item), "tag");
            if (tag_val && yyjson_is_str(tag_val)) {
                sum += static_cast<uint64_t>(yyjson_get_str_len(tag_val));
            }

            // Process 'id'
            yyjson_val *id_val = yyjson_obj_get(yyjson_obj(item), "id");
            if (id_val && yyjson_is_num(id_val)) {
                sum += static_cast<uint64_t>(yyjson_get_uint(id_val));
            }

            // Process 'value'
            yyjson_val *value_val = yyjson_obj_get(yyjson_obj(item), "value");
            if (value_val && yyjson_is_num(value_val)) {
                sum += static_cast<uint64_t>(yyjson_get_uint(value_val));
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}