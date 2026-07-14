#include "interface.h"
#include <yyjson.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_read_stage stage;
    yyjson_read_err err;
    
    // Initialize the on-demand reader
    if (!yyjson_read_init(&stage, json_input.c_str(), json_input.size(), &err)) {
        return 0;
    }

    uint64_t sum = 0;

    // We expect an array at the root
    // yyjson_read_stage allows us to traverse without building a full DOM
    // However, for simplicity and speed in this specific schema, 
    // we use the DOM parser but with yyjson's highly optimized engine.
    // yyjson is significantly faster than nlohmann for large inputs.
    
    yyjson_doc *doc = yyjson_read_doc(&stage, &err);
    if (!doc) {
        return 0;
    }

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (yyjson_is_arr(root)) {
        yyjson_val *item;
        yyjson_iter iter;
        yyjson_iter_init(&iter, root);
        
        while ((item = yyjson_iter_next(&iter))) {
            // Extract 'id'
            yyjson_val *id_val = yyjson_obj_get(item, "id");
            if (id_val && yyjson_is_u64(id_val)) {
                sum += yyjson_u64(id_val);
            }

            // Extract 'nested' object
            yyjson_val *nested = yyjson_obj_get(item, "nested");
            if (nested && yyjson_is_obj(nested)) {
                // Extract 'a'
                yyjson_val *a_val = yyjson_obj_get(nested, "a");
                if (a_val && yyjson_is_u64(a_val)) {
                    sum += yyjson_u64(a_val);
                }
                // Extract 'b'
                yyjson_val *b_val = yyjson_obj_get(nested, "b");
                if (b_val && yyjson_is_u64(b_val)) {
                    sum += yyjson_u64(b_val);
                }
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}