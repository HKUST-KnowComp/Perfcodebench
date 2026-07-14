#include "interface.h"
#include <yyjson.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    std::string mutable_input = json_input;
    yyjson_doc* doc = yyjson_read(mutable_input.data(), mutable_input.size(), YYJSON_READ_INSITU);
    if (!doc) {
        return 0;
    }
    yyjson_val* root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;
    size_t len = yyjson_arr_size(root);
    for (size_t i = 0; i < len; ++i) {
        yyjson_val* item = yyjson_arr_get(root, i);
        yyjson_val* id_val = yyjson_obj_get(item, "id");
        sum += yyjson_get_uint(id_val);
        yyjson_val* nested_val = yyjson_obj_get(item, "nested");
        yyjson_val* a_val = yyjson_obj_get(nested_val, "a");
        sum += yyjson_get_uint(a_val);
        yyjson_val* b_val = yyjson_obj_get(nested_val, "b");
        sum += yyjson_get_uint(b_val);
    }
    yyjson_doc_free(doc);
    return sum;
}