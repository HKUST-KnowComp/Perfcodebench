#include "interface.h"
#include <yyjson.h>
#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    // Parse with yyjson for speed
    yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) {
        return 0;
    }
    
    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }
    
    uint64_t sum = 0;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);
    yyjson_val* val;
    while ((val = yyjson_arr_iter_next(&iter)) != NULL) {
        if (!yyjson_is_obj(val)) continue;
        
        // Check flag
        yyjson_val* flag_val = yyjson_obj_get(val, "flag");
        if (!flag_val || !yyjson_get_bool(flag_val)) continue;
        
        // Get required fields
        yyjson_val* id_val = yyjson_obj_get(val, "id");
        yyjson_val* value_val = yyjson_obj_get(val, "value");
        yyjson_val* name_val = yyjson_obj_get(val, "name");
        
        if (!id_val || !value_val || !name_val) continue;
        
        uint64_t id = yyjson_get_uint(id_val);
        uint64_t value = yyjson_get_uint(value_val);
        // Length of name string – avoids copying
        size_t name_len = yyjson_get_len(name_val);
        
        sum += id + value + name_len;
    }
    
    yyjson_doc_free(doc);
    return sum;
}
