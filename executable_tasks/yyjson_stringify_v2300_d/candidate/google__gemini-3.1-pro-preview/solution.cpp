#include "interface.h"
#include "yyjson.h"
#include <vector>
#include <string>
#include <cstdint>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    size_t n = ids.size();
    if (n == 0) {
        return "[]";
    }

    // Pre-calculate memory requirements for yyjson nodes to minimize allocations.
    // Each record has 1 object, 4 keys, and 4 values = 9 nodes.
    // Plus 1 node for the root array.
    size_t max_nodes = n * 9 + 1;
    
    // Estimate string memory: keys ("id", "flag", "value", "name") are small.
    // We use the mutable API which will copy strings into its own pool.
    // A safe estimate for the pool size is nodes * sizeof(yyjson_mut_val) + string data.
    size_t pool_size = max_nodes * sizeof(yyjson_mut_val) + (n * 64); 
    void* buf = malloc(pool_size);
    
    yyjson_alc alc;
    yyjson_alc_pool_init(&alc, buf, pool_size);
    
    yyjson_mut_doc *doc = yyjson_mut_doc_new(&alc);
    yyjson_mut_val *root = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, root);

    for (size_t i = 0; i < n; ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        
        // Field order: id, flag, value, name
        // yyjson_mut_obj_add_* functions preserve insertion order.
        uint32_t id = ids[i];
        yyjson_mut_obj_add_uint(doc, obj, "id", id);
        yyjson_mut_obj_add_bool(doc, obj, "flag", (id % 3U) == 0U);
        yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
        
        // Use the string length to avoid redundant strlen calls
        const std::string& name = names[i];
        yyjson_mut_obj_add_strncpy(doc, obj, "name", name.c_str(), name.length());
        
        yyjson_mut_arr_append(root, obj);
    }

    // Write to minified JSON string
    size_t len = 0;
    char *json_ptr = yyjson_mut_write(doc, 0, &len);
    
    std::string result;
    if (json_ptr) {
        result.assign(json_ptr, len);
        // yyjson_mut_write uses the default allocator (malloc) for the output string
        free(json_ptr);
    }

    yyjson_mut_doc_free(doc);
    free(buf);
    
    return result;
}