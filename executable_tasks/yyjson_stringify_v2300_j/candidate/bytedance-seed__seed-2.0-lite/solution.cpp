#include "interface.h"
#include <yyjson.h>
#include <string>
#include <cstdlib>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    yyjson_doc* doc = yyjson_doc_new();
    yyjson_val* root = yyjson_arr_new(doc);
    yyjson_doc_set_root(doc, root);

    const size_t count = ids.size();
    for (size_t i = 0; i < count; ++i) {
        yyjson_val* obj = yyjson_obj_new(doc);
        yyjson_obj_add_int(obj, "id", static_cast<long long>(ids[i]));
        const bool flag = (ids[i] % 3U) == 0U;
        yyjson_obj_add_bool(obj, "flag", flag);
        yyjson_obj_add_int(obj, "value", static_cast<long long>(values[i]));
        yyjson_obj_add_str(obj, "name", names[i].c_str());
        yyjson_arr_append(root, obj);
    }

    size_t len;
    char* json_str = yyjson_write(doc, YYJSON_WRITE_MINIFY, &len);
    std::string result(json_str, len);
    free(json_str);
    yyjson_doc_free(doc);

    return result;
}