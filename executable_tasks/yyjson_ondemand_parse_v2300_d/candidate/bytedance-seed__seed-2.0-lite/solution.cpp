#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;
    yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) return 0;
    
    yyjson_val* root = yyjson_doc_get_root(doc);
    const size_t item_count = yyjson_arr_size(root);
    
    for (size_t i = 0; i < item_count; ++i) {
        yyjson_val* item = yyjson_arr_get(root, i);
        sum += yyjson_get_uint64(yyjson_obj_get(item, "id"));
        yyjson_val* nested = yyjson_obj_get(item, "nested");
        sum += yyjson_get_uint64(yyjson_obj_get(nested, "a"));
        sum += yyjson_get_uint64(yyjson_obj_get(nested, "b"));
    }
    
    yyjson_doc_free(doc);
    return sum;
}