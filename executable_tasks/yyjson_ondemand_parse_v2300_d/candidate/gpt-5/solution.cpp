#include "interface.h"

#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
  // Make a mutable copy to enable yyjson in-situ parsing (faster, fewer allocations)
  std::string buffer = json_input;

  yyjson_read_err err;
  yyjson_doc* doc = yyjson_read_opts(buffer.data(), buffer.size(),
                                     YYJSON_READ_INSITU | YYJSON_READ_STOP_WHEN_DONE,
                                     nullptr, &err);
  if (!doc) {
    return 0; // If input is invalid, return 0 (inputs are expected valid in the harness)
  }

  yyjson_val* root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;

  // Expect root to be an array of objects
  if (yyjson_is_arr(root)) {
    yyjson_val* item;
    size_t idx, max;
    // Precompute key lengths to avoid strlen in lookups
    static const char k_id[] = "id";      // len = 2
    static const char k_nested[] = "nested"; // len = 6
    static const char k_a[] = "a";        // len = 1
    static const char k_b[] = "b";        // len = 1

    yyjson_arr_foreach(root, idx, item) {
      // item is an object with keys: id, nested:{a,b}
      yyjson_val* id = yyjson_obj_getn(item, k_id, 2);
      yyjson_val* nested = yyjson_obj_getn(item, k_nested, 6);
      yyjson_val* a = yyjson_obj_getn(nested, k_a, 1);
      yyjson_val* b = yyjson_obj_getn(nested, k_b, 1);

      // Assuming valid input per task, direct numeric extraction
      sum += yyjson_get_uint(id);
      sum += yyjson_get_uint(a);
      sum += yyjson_get_uint(b);
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
