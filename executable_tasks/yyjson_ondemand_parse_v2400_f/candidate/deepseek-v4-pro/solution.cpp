#include "interface.h"

#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.length(), 0);
    if (!doc) return 0;
    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) { yyjson_doc_free(doc); return 0; }
    uint64_t sum = 0;
    size_t idx, max;
    yyjson_val *item;
    yyjson_arr_foreach(root, idx, max, item) {
        sum += yyjson_get_uint(yyjson_obj_get(item, "id"));
        yyjson_val *nested = yyjson_obj_get(item, "nested");
        if (nested) {
            sum += yyjson_get_uint(yyjson_obj_get(nested, "a"));
            sum += yyjson_get_uint(yyjson_obj_get(nested, "b"));
        }
    }
    yyjson_doc_free(doc);
    return sum;
}
