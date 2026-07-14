#include "interface.h"
#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read_capture(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;

    if (yyjson_get_type(root) == YYJSON_TYPE_ARRAY) {
        yyjson_val *item = root;
        while (item) {
            // Extract "id"
            yyjson_val *id_val = yyjson_obj_get(item, "id");
            if (id_val) sum += yyjson_get_uint64(id_val);

            // Extract "x"
            yyjson_val *x_val = yyjson_obj_get(item, "x");
            if (x_val) sum += yyjson_get_uint64(x_val);

            // Extract "vals"[1]
            yyjson_val *vals_val = yyjson_obj_get(item, "vals");
            if (vals_val && yyjson_get_type(vals_val) == YYJSON_TYPE_ARRAY) {
                yyjson_val *v1 = yyjson_arr_get(vals_val, 1);
                if (v1) sum += yyjson_get_uint64(v1);
            }

            item = yyjson_val_next(item);
        }
    }

    yyjson_doc_free(doc);
    return sum;
}