#include "interface.h"
#include <yyjson.h>
#include <cstddef>
#include <string>
#include <vector>

std::string build_json(const std::vector<uint32_t>& ids, const std::vector<uint32_t>& values, const std::vector<std::string>& names) {
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *root = yyjson_mut_arr(doc);
    const std::size_t n = ids.size();
    for (std::size_t i = 0; i < n; ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "id"),   yyjson_mut_uint(doc, ids[i]));
        yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "flag"), yyjson_mut_bool(doc, (ids[i] % 3U) == 0U));
        yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "value"),yyjson_mut_uint(doc, values[i]));
        yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "name"), yyjson_mut_strn(doc, names[i].data(), names[i].size()));
        yyjson_mut_arr_append(root, obj);
    }
    std::size_t len = 0;
    const char *json = yyjson_mut_write(doc, 0, &len);
    std::string result(json, len);
    yyjson_mut_doc_free(doc);
    return result;
}