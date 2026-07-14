#include "interface.h"
#include "yyjson.h"
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read_opts(json_input.c_str(), json_input.size(), 0);
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
        if (!flag_val || !yyjson_get_bool(flag_val)) {
            continue;
        }

        yyjson_val *tag_val = yyjson_obj_get(item, "tag");
        if (tag_val) {
            sum += yyjson_get_len(tag_val);
        }

        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (id_val) {
            sum += yyjson_get_uint64(id_val);
        }

        yyjson_val *val_val = yyjson_obj_get(item, "value");
        if (val_val) {
            sum += yyjson_get_uint64(val_val);
        }
    }

    yyjson_doc_free(doc);
    return sum;
}