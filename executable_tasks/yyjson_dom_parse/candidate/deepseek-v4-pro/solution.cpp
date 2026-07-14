#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.length(), 0);
    yyjson_val* root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;
    size_t idx, max;
    yyjson_val* item;
    yyjson_arr_foreach(root, idx, max, item) {
        yyjson_val* id_val = yyjson_obj_get(item, "id");
        yyjson_val* x_val = yyjson_obj_get(item, "x");
        yyjson_val* vals_val = yyjson_obj_get(item, "vals");
        yyjson_val* val1 = yyjson_arr_get(vals_val, 1);
        sum += yyjson_get_uint(id_val);
        sum += yyjson_get_uint(x_val);
        sum += yyjson_get_uint(val1);
    }
    yyjson_doc_free(doc);
    return sum;
}