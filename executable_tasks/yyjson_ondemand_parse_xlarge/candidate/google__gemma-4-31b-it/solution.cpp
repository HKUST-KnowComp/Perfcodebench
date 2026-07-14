#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;
    yyjson_doc *doc = yyjson_read_opts(json_input.c_str(), json_input.size(), 0, NULL);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (yyjson_get_type(root) != YYJSON_ARRAY) {
        yyjson_doc_free(doc);
        return 0;
    }

    yyjson_val *item = root;
    while ((item = yyjson_arr_next(item)) != NULL) {
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (id_val) sum += yyjson_get_uint64(id_val);

        yyjson_val *nested = yyjson_obj_get(item, "nested");
        if (nested) {
            yyjson_val *a_val = yyjson_obj_get(nested, "a");
            if (a_val) sum += yyjson_get_uint64(a_val);
            
            yyjson_val *b_val = yyjson_obj_get(nested, "b");
            if (b_val) sum += yyjson_get_uint64(b_val);
        }
    }

    yyjson_doc_free(doc);
    return sum;
}