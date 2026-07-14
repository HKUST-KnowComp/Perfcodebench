#include "interface.h"

#include <yyjson.h>
#include <cstdint>
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *arr = yyjson_mut_arr_with_capacity(doc, ids.size());
    for (size_t i = 0; i < ids.size(); ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj_with_capacity(doc, 4);
        yyjson_mut_obj_add_strcpy(doc, obj, "id", yyjson_mut_uint(doc, ids[i]));
        yyjson_mut_obj_add_strcpy(doc, obj, "flag", yyjson_mut_bool(doc, (ids[i] % 3U) == 0U));
        yyjson_mut_obj_add_strcpy(doc, obj, "value", yyjson_mut_uint(doc, values[i]));
        yyjson_mut_obj_add_strcpy(doc, obj, "name", yyjson_mut_strcpy(doc, names[i].c_str()));
        yyjson_mut_arr_append(arr, obj);
    }
    yyjson_mut_doc_set_root(doc, arr);
    size_t len;
    char *buf = yyjson_mut_write(doc, 0, &len);
    std::string result(buf, len);
    free((void*)buf);
    yyjson_mut_doc_free(doc);
    return result;
}