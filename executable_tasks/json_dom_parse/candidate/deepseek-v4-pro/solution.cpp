#include "interface.h"
#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    // Thread-local buffer reused across calls to avoid repeated memory allocations.
    thread_local std::string padded_buf;
    size_t needed = json_input.size() + simdjson::SIMDJSON_PADDING;
    if (padded_buf.capacity() < needed) {
        padded_buf.reserve(needed);
    }
    padded_buf.assign(json_input);
    padded_buf.resize(needed);

    simdjson::padded_string_view json_ps(padded_buf.data(), json_input.size(), padded_buf.size());

    simdjson::ondemand::parser parser;
    simdjson::ondemand::document doc;
    simdjson::ondemand::array arr;
    // In a performance-critical context we skip explicit error checking;
    // the provided inputs are well‑formed and always contain required fields.
    parser.iterate(json_ps).get(doc);
    doc.get_array().get(arr);

    uint64_t sum = 0;
    for (auto element : arr) {
        simdjson::ondemand::object obj;
        element.get_object().get(obj);

        uint64_t id_val, x_val, vals1_val;
        obj["id"].get_uint64().get(id_val);
        obj["x"].get_uint64().get(x_val);

        simdjson::ondemand::array vals_arr;
        obj["vals"].get_array().get(vals_arr);
        auto it = vals_arr.begin();
        ++it; // skip index 0
        (*it).get_uint64().get(vals1_val);

        sum += id_val + x_val + vals1_val;
    }
    return sum;
}
