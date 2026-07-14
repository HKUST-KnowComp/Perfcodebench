#include "interface.h"
#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;

    if (json_input.empty()) {
        return 0;
    }

    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
    if (!doc) {
        return 0;
    }

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (root && yyjson_is_arr(root)) {
        size_t idx;
        yyjson_val *obj;
        yyjson_arr_foreach(root, idx, obj) {
            // id
            yyjson_val *idv = yyjson_obj_get(obj, "id");
            sum += yyjson_get_uint(idv);

            // nested -> a, b
            yyjson_val *nested = yyjson_obj_get(obj, "nested");
            if (nested && yyjson_is_obj(nested)) {
                yyjson_val *a = yyjson_obj_get(nested, "a");
                yyjson_val *b = yyjson_obj_get(nested, "b");
                sum += yyjson_get_uint(a);
                sum += yyjson_get_uint(b);
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}
