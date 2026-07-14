#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
    if (!doc) return 0;
    
    uint64_t sum = 0;
    yyjson_val* root = yyjson_doc_get_root(doc);
    const size_t item_count = yyjson_arr_size(root);
    
    for (size_t i = 0; i < item_count; ++i) {
        yyjson_val* item = yyjson_arr_get(root, i);
        sum += yyjson_get_uint(yyjson_obj_get(item, "id"));
        sum += yyjson_get_uint(yyjson_obj_get(item, "x"));
        yyjson_val* vals = yyjson_obj_get(item, "vals");
        sum += yyjson_get_uint(yyjson_arr_get(vals, 1));
    }
    
    yyjson_doc_free(doc);
    return sum;
}