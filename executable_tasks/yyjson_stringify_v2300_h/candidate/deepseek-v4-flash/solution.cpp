#include "interface.h"
#include <yyjson.h>
#include <cstdlib>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    if (!doc) {
        return "[]";
    }
    yyjson_mut_val *arr = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, arr);

    for (std::size_t i = 0; i < ids.size(); ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);

        yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
        bool flag = (ids[i] % 3U) == 0U;
        yyjson_mut_obj_add_bool(doc, obj, "flag", flag);
        yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
        yyjson_mut_obj_add_strcpy(doc, obj, "name", names[i].c_str(), names[i].size());

        yyjson_mut_arr_append(doc, arr, obj);
    }

    size_t len;
    char *json_str = yyjson_mut_write(doc, YYJSON_WRITE_NOFLAG, &len);
    if (!json_str) {
        yyjson_mut_doc_free(doc);
        return "[]";
    }
    std::string result(json_str, len);
    free(json_str);
    yyjson_mut_doc_free(doc);
    return result;
}