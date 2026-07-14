#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include "yyjson.h"

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
  yyjson_mut_val* root = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, root);

  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    yyjson_mut_val* obj = yyjson_mut_obj(doc);

    // id
    yyjson_mut_obj_add(obj,
                       yyjson_mut_str(doc, "id"),
                       yyjson_mut_uint(doc, static_cast<uint64_t>(ids[i])));

    // flag
    bool flag = (ids[i] % 3u) == 0u;
    yyjson_mut_obj_add(obj,
                       yyjson_mut_str(doc, "flag"),
                       yyjson_mut_bool(doc, flag));

    // value
    yyjson_mut_obj_add(obj,
                       yyjson_mut_str(doc, "value"),
                       yyjson_mut_uint(doc, static_cast<uint64_t>(values[i])));

    // name
    const std::string& nm = names[i];
    yyjson_mut_obj_add(obj,
                       yyjson_mut_str(doc, "name"),
                       yyjson_mut_strn(doc, nm.data(), nm.size()));

    yyjson_mut_arr_add(root, obj);
  }

  size_t len = 0;
  char* json = yyjson_mut_write(doc, 0, &len, nullptr); // minified by default

  std::string out;
  if (json && len) out.assign(json, len);
  else out.clear();

  yyjson_free(json);
  yyjson_mut_doc_free(doc);
  return out;
}
