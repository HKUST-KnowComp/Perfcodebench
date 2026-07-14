#include "interface.h"
#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    uint64_t sum = 0;
    if (doc) {
        yyjson_val *root = yyjson_doc_get_root(doc);
        size_t idx, max;
        yyjson_val *item;
        yyjson_arr_foreach(root, idx, max, item) {
            sum += yyjson_get_uint(yyjson_obj_getn(item, "id", 2));
            yyjson_val *nested = yyjson_obj_getn(item, "nested", 6);
            if (nested) {
                sum += yyjson_get_uint(yyjson_obj_getn(nested, "a", 1));
                sum += yyjson_get_uint(yyjson_obj_getn(nested, "b", 1));
            }
        }
        yyjson_doc_free(doc);
    }
    return sum;
}
