#include "interface.h"
#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) return 0;

    uint64_t sum = 0;
    yyjson_val* root = yyjson_doc_get_root(doc);

    if (yyjson_is_arr(root)) {
        size_t idx, max;
        yyjson_val* item;
        yyjson_arr_foreach(root, idx, max, item) {
            // Access fields by key using yyjson_obj_get
            yyjson_val* id = yyjson_obj_get(item, "id");
            yyjson_val* x = yyjson_obj_get(item, "x");
            yyjson_val* vals = yyjson_obj_get(item, "vals");

            if (id) sum += yyjson_get_uint(id);
            if (x) sum += yyjson_get_uint(x);
            if (vals && yyjson_is_arr(vals) && yyjson_arr_size(vals) > 1) {
                sum += yyjson_get_uint(yyjson_arr_get(vals, 1));
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}