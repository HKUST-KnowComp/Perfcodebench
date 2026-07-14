#include "interface.h"

#include <cstdlib>
#include "yyjson.h"

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  // Create a mutable JSON document with an array root
  yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
  if (!doc) return std::string();
  yyjson_mut_val* root = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, root);

  const size_t n = ids.size();
  for (size_t i = 0; i < n; ++i) {
    yyjson_mut_val* obj = yyjson_mut_obj(doc);

    // id
    yyjson_mut_obj_add(doc, obj, "id", yyjson_mut_uint(doc, static_cast<uint64_t>(ids[i])));

    // flag: true if id % 3 == 0
    const bool flag = ((ids[i] % 3u) == 0u);
    yyjson_mut_obj_add(doc, obj, "flag", yyjson_mut_bool(doc, flag));

    // value
    yyjson_mut_obj_add(doc, obj, "value", yyjson_mut_uint(doc, static_cast<uint64_t>(values[i])));

    // name
    const std::string& nm = names[i];
    yyjson_mut_obj_add(doc, obj, "name", yyjson_mut_strn(doc, nm.data(), nm.size()));

    yyjson_mut_arr_add(root, obj);
  }

  size_t len = 0;
  char* buf = yyjson_mut_write(doc, 0, &len); // minified by default
  std::string out;
  if (buf) {
    out.assign(buf, len);
    std::free(buf);
  }

  yyjson_mut_doc_free(doc);
  return out;
}
