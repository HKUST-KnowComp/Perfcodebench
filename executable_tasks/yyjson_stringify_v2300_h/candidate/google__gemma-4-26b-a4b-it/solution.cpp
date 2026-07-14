#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    size_t n = ids.size();
    yyjson_doc *doc = yyjson_doc_new(0);
    if (!doc) return "[]";

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *arr = yyjson_arr(root);

    for (size_t i = 0; i < n; ++i) {
        yyjson_val *obj = yyjson_obj(root);
        
        // Field order must be preserved: id, flag, value, name
        yyjson_write(doc, obj, "id", yyjson_u32(doc, ids[i]));
        yyjson_write(doc, obj, "flag", yyjson_bool(doc, (ids[i] % 3U) == 0U));
        yyjson_write(doc, obj, "value", yyjson_u32(doc, values[i]));
        yyjson_write(doc, obj, "name", yyjson_str(doc, names[i].c_str()));
        
        yyjson_arr_append(arr, obj);
    }

    yyjson_writer *writer = yyjson_writer_new();
    yyjson_write_doc(writer, doc);
    
    size_t len = yyjson_writer_len(writer);
    std::string result;
    result.resize(len);
    yyjson_writer_write_to_buffer(writer, reinterpret_cast<char*>(result.data()));

    yyjson_writer_free(writer);
    yyjson_doc_free(doc);

    return result;
}