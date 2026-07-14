#include "interface.h"

#include <cstdint>
#include <string>
#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_read_err err;
  yyjson_doc* doc = yyjson_read_opts(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG, nullptr, &err);
  if (!doc) {
    return 0;
  }

  yyjson_val* root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;

  if (yyjson_is_arr(root)) {
    yyjson_arr_iter it;
    if (yyjson_arr_iter_init(root, &it)) {
      yyjson_val* obj;
      while ((obj = yyjson_arr_iter_next(&it))) {
        // Expect object with fields: id (uint), nested { a (uint), b (uint) }
        if (!yyjson_is_obj(obj)) continue;

        yyjson_val* id_v = yyjson_obj_getn(obj, "id", 2);
        if (id_v) sum += yyjson_get_uint(id_v);

        yyjson_val* nested_v = yyjson_obj_getn(obj, "nested", 6);
        if (nested_v && yyjson_is_obj(nested_v)) {
          yyjson_val* a_v = yyjson_obj_getn(nested_v, "a", 1);
          if (a_v) sum += yyjson_get_uint(a_v);
          yyjson_val* b_v = yyjson_obj_getn(nested_v, "b", 1);
          if (b_v) sum += yyjson_get_uint(b_v);
        }
      }
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
