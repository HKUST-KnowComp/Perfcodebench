#include "interface.h"
#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    yyjson_val *root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;
    yyjson_val *item;
    size_t idx, max;
    yyjson_arr_foreach(root, idx, max, item) {
        uint64_t id = yyjson_get_uint(yyjson_obj_get(item, "id"));
        yyjson_val *nested = yyjson_obj_get(item, "nested");
        uint64_t a = yyjson_get_uint(yyjson_obj_get(nested, "a"));
        uint64_t b = yyjson_get_uint(yyjson_obj_get(nested, "b"));
        sum += id + a + b;
    }
    yyjson_doc_free(doc);
    return sum;
}