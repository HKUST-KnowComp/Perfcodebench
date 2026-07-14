#include "interface.h"
#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read_start(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_val_get_type(root) == YYJSON_TYPE_ARR) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    yyjson_val *item = root;
    while ((item = yyjson_val_next(item))) {
        // Extract "id"
        yyjson_val *id_val = yyjson_val_get_proj(item, "id");
        if (id_val) sum += (uint64_t)yyjson_val_get_int(id_val);

        // Extract "nested"
        yyjson_val *nested_val = yyjson_val_get_proj(item, "nested");
        if (nested_val) {
            yyjson_val *a_val = yyjson_val_get_proj(nested_val, "a");
            if (a_val) sum += (uint64_t)yyjson_val_get_int(a_val);
            
            yyjson_val *b_val = yyjson_val_get_proj(nested_val, "b");
            if (b_val) sum += (uint64_t)yyjson_val_get_int(b_val);
        }
    }

    yyjson_doc_free(doc);
    return sum;
}