#include "interface.h"
#include <yyjson.h>
#include <vector>
#include <cstddef>

uint64_t compute_checksum(const std::string& json_input) {
    // Use in-situ parsing: copy the input into a mutable buffer.
    std::vector<char> buf(json_input.begin(), json_input.end());
    buf.push_back('\0'); // null-terminator not strictly required for in-situ, but safe.
    yyjson_doc *doc = yyjson_read_insitu(buf.data(), json_input.size(), 0);
    if (!doc) return 0; // fallback on error (should not happen for valid JSON)
    
    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }
    
    uint64_t sum = 0;
    yyjson_val *item;
    size_t idx, max;
    yyjson_arr_foreach(root, idx, max, item) {
        // id field
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (yyjson_is_uint(id_val)) {
            sum += yyjson_get_uint(id_val);
        }
        // nested object
        yyjson_val *nested = yyjson_obj_get(item, "nested");
        if (yyjson_is_obj(nested)) {
            yyjson_val *a = yyjson_obj_get(nested, "a");
            if (yyjson_is_uint(a)) {
                sum += yyjson_get_uint(a);
            }
            yyjson_val *b = yyjson_obj_get(nested, "b");
            if (yyjson_is_uint(b)) {
                sum += yyjson_get_uint(b);
            }
        }
    }
    
    yyjson_doc_free(doc);
    return sum;
}