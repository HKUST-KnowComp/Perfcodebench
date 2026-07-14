#include "interface.h"
#include "yyjson.h"
#include <vector>

uint64_t compute_checksum(const std::string& json_input) {
    if (json_input.empty()) return 0;

    // Use a stack-based allocator to avoid heap allocations for small to medium JSONs.
    // 64KB is typically enough for 'small nested' tasks.
    char stack_buffer[64 * 1024];
    yyjson_alc alc;
    yyjson_alc_pool_init(&alc, stack_buffer, sizeof(stack_buffer));

    // Parse the JSON input.
    yyjson_doc* doc = yyjson_read_opts(const_cast<char*>(json_input.data()), json_input.size(), 0, &alc, nullptr);
    if (!doc) return 0;

    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    yyjson_val* item;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);

    // Iterate through the array of objects.
    while ((item = yyjson_arr_iter_next(&iter))) {
        // Extract 'id' from the top-level object.
        yyjson_val* id_val = yyjson_obj_get(item, "id");
        if (id_val) {
            sum += yyjson_get_uint(id_val);
        }

        // Extract 'nested' object.
        yyjson_val* nested_obj = yyjson_obj_get(item, "nested");
        if (nested_obj) {
            // Extract 'a' and 'b' from the nested object.
            yyjson_val* a_val = yyjson_obj_get(nested_obj, "a");
            if (a_val) {
                sum += yyjson_get_uint(a_val);
            }
            yyjson_val* b_val = yyjson_obj_get(nested_obj, "b");
            if (b_val) {
                sum += yyjson_get_uint(b_val);
            }
        }
    }

    // Free the document (though with the pool allocator, this is very fast).
    yyjson_doc_free(doc);
    return sum;
}