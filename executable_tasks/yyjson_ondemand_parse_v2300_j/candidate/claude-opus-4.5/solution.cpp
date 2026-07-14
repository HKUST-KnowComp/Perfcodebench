#include "interface.h"

#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) return 0;
    
    yyjson_val *root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;
    
    size_t idx, max;
    yyjson_val *item;
    yyjson_arr_foreach(root, idx, max, item) {
        yyjson_val *id = yyjson_obj_get(item, "id");
        sum += yyjson_get_uint(id);
        
        yyjson_val *nested = yyjson_obj_get(item, "nested");
        yyjson_val *a = yyjson_obj_get(nested, "a");
        yyjson_val *b = yyjson_obj_get(nested, "b");
        sum += yyjson_get_uint(a);
        sum += yyjson_get_uint(b);
    }
    
    yyjson_doc_free(doc);
    return sum;
}